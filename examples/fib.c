#include <stdio.h>
#include "generator.h"

void fib(void *arg)
{
    long max = (long)arg;
    long a = 0;
    long b = 1;
    while (a < max) {
        generator_yield((void*)a);
        long c = a + b;
        a = b;
        b = c;
    }
}

int main()
{
    Generator *g = generator_create(fib);
    foreach (value, g, (void*)(1000*1000)) {
        printf("%ld\n", (long)value);
    }

    generator_destroy(g);
}
