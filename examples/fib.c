#include <stdio.h>
#include "generator.h"

void fib(void *arg)
{
    long max = (long)arg;
    long a = 0;
    long b = 1;
    while (a < max) {
        yield(a);
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
    printf("Final G: %p, %p\n", g->rsp, g->stack_base);
    /* generator_destroy(g); */
    
    /* Generator *g2 = generator_create(fib); */
    /* foreach (value, g2, (void*)(1000*1000)) { */
    /*     printf("%ld\n", (long)value); */
    /* } */
    /* generator_destroy(g2); */

}
