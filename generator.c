#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <threads.h>

#include <sys/mman.h>
#include <unistd.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#include "generator.h"

#define da_last(da) (NOB_ASSERT((da)->count > 0), (da)->items[(da)->count-1])

#define GENERATOR_STACK_CAPACITY (1024*getpagesize())

typedef struct {
    Generator **items;
    size_t count;
    size_t capacity;
} Generator_Stack;

thread_local Generator_Stack generator_stack = {0};

void generator_init(void)
{
    Generator *g = malloc(sizeof(Generator));
    assert(g != NULL && "Buy more RAM lol");
    memset(g, 0, sizeof(*g));
    da_append(&generator_stack, g);
}

// Linux x86_64 call convention
// %rdi, %rsi, %rdx, %rcx, %r8, and %r9

void* __attribute__((naked)) generator_next(Generator *g, void *arg)
{
    UNUSED(g);
    UNUSED(arg);
    // @arch
    if(g->dead){asm("xor %rax, %rax\n ret\n");}
    asm(
    "    pushq %rdi\n"
    "    pushq %rbp\n"
    "    pushq %rbx\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    movq %rsp, %rdx\n"     // rsp
    "    jmp generator_switch_context\n");
}

void __attribute__((naked)) generator_restore_context(void *rsp)
{
    // @arch
    (void)rsp;
    asm(
    "    movq %rdi, %rsp\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbx\n"
    "    popq %rbp\n"
    "    popq %rdi\n"
    "    ret\n");
}

void __attribute__((naked)) generator_restore_context_with_return(void *rsp, void *arg)
{
    // @arch
    UNUSED(rsp);
    UNUSED(arg);
    asm(
    "    movq %rdi, %rsp\n"
    "    movq %rsi, %rax\n"
    "    popq %r15\n"
    "    popq %r14\n"
    "    popq %r13\n"
    "    popq %r12\n"
    "    popq %rbx\n"
    "    popq %rbp\n"
    "    popq %rdi\n"
    "    ret\n");
}

void generator_switch_context(Generator *g, void *arg, void *rsp)
{
    da_last(&generator_stack)->rsp = rsp;
    da_append(&generator_stack, g);
    if (g->fresh) {
        g->fresh = false;
        // ******************************
        // ^                          ^rsp
        // stack_base
        void **rsp = (void**)((char*)g->stack_base + GENERATOR_STACK_CAPACITY);
        *(rsp-3) = arg;
        generator_restore_context(g->rsp);
    } else {
        generator_restore_context_with_return(g->rsp, arg);
    }
}

void *__attribute__((naked)) generator_yield(void *arg)
{
    UNUSED(arg);
    // @arch
    asm(
    "    pushq %rdi\n"
    "    pushq %rbp\n"
    "    pushq %rbx\n"
    "    pushq %r12\n"
    "    pushq %r13\n"
    "    pushq %r14\n"
    "    pushq %r15\n"
    "    movq %rsp, %rsi\n"     // rsp
    "    jmp generator_return\n");
}

void generator_return(void *arg, void *rsp)
{
    da_last(&generator_stack)->rsp = rsp;
    generator_stack.count -= 1;
    generator_restore_context_with_return(da_last(&generator_stack)->rsp, arg);
}

void generator__finish_current(void)
{
    da_last(&generator_stack)->dead = true;
    generator_stack.count -= 1;
    generator_restore_context_with_return(da_last(&generator_stack)->rsp, NULL);
}

Generator *generator_create(void (*f)(void*))
{
    Generator *g = malloc(sizeof(Generator));
    assert(g != NULL && "Buy more RAM lol");
    memset(g, 0, sizeof(*g));

    g->stack_base = mmap(NULL, GENERATOR_STACK_CAPACITY, PROT_WRITE|PROT_READ, MAP_PRIVATE|MAP_STACK|MAP_ANONYMOUS|MAP_GROWSDOWN, -1, 0);
    assert(g->stack_base != MAP_FAILED);
    void **rsp = (void**)((char*)g->stack_base + GENERATOR_STACK_CAPACITY);
    *(--rsp) = generator__finish_current;
    *(--rsp) = f;
    *(--rsp) = 0;   // push rdi
    *(--rsp) = 0;   // push rbx
    *(--rsp) = 0;   // push rbp
    *(--rsp) = 0;   // push r12
    *(--rsp) = 0;   // push r13
    *(--rsp) = 0;   // push r14
    *(--rsp) = 0;   // push r15
    g->rsp = rsp;
    g->fresh = true;
    return g;
}

void generator_destroy(Generator *g)
{
    munmap(g->stack_base, GENERATOR_STACK_CAPACITY);
    free(g);
}
