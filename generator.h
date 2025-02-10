#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <stdbool.h>

typedef struct {
    void *rsp;
    void *stack_base;
    bool dead;
    bool fresh;
} Generator;

void generator_init(void);
void* generator_next(Generator *g, void *arg);
void* generator_yield(void *arg);
Generator *generator_create(void (*f)(void*));
void generator_destroy(Generator *g);

#define foreach(it, g, arg) for (void *it = generator_next(g, arg); !(g)->dead; it = generator_next(g, arg))

#endif // GENERATOR_H_
