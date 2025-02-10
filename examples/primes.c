#include <stdio.h>
#include "generator.h"

void primes(void *arg)
{
    (void) arg;

    generator_yield((void*)2);

    for (long n = 3; true; n += 2) {
        Generator *primes_gen = generator_create(primes);

        bool is_prime = true;
        foreach (value, primes_gen, NULL) {
            long p = (long)value;

            if (p * p > n) break;

            if (n % p == 0) {
                is_prime = false;
                break;
            }
        }

        generator_destroy(primes_gen);

        if (is_prime) generator_yield((void*)n);
    }
}

int main()
{
    generator_init();

    Generator *g = generator_create(primes);
    foreach (value, g, NULL) {
        printf("%ld\n", (long)value);
    }

    generator_destroy(g);
}
