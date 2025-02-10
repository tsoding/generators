#include <stdio.h>
#include <stdbool.h>
#include "generator.h"

void square(void *arg)
{
    while (true) {
        long x = (long)arg;
        arg = yield(x*x);
    }
}

int main()
{
    Generator *g = generator_create(square);
    for (long x = 0; x < 100; ++x) {
        long xx = (long)generator_next(g, (void*)x);
        printf("%ld\n", xx);
    }
    generator_destroy(g);
    return 0;
}
