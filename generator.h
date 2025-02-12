#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <stdbool.h>

typedef struct {
    void *rsp;
    void *stack_base;
    bool dead;
    bool fresh;
} Generator;

// void generator_init(void);
void* generator_next(Generator *g, void *arg);
void* generator_yield(void *arg);
Generator *generator_create(void (*f)(void*));

/**
 * Checks if a generator has finished, and also cleans Up the generators memory
 **/
bool generator_finished(Generator *g);

/**
 * Forces the cleanup of a generator memory
 **/
void generator_destroy(Generator *g);

#define foreach(it, g, arg) for (void *it = generator_next(g, arg); !generator_finished(g); it = generator_next(g, arg))
#define yield(x) generator_yield((void*)(x))

#endif // GENERATOR_H_
