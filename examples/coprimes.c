#include <stdio.h>
#include "generator.h"

void coprimes(void *imax_arg){
    unsigned long m,n,aux;
    unsigned long imax = (unsigned long)imax_arg;
    int br;

    if(imax<2){ return; }

    m=2;
    n=1;
    generator_yield((void*)m);
    generator_yield((void*)n);

    if(imax<3){ return; }

    br=0;
    while(m!=1){
        aux=(m<<1)-n;
        if(aux<=imax && br<1){
            aux = m;
            m=(m<<1)-n;
            n=aux;
            br=0;
            generator_yield((void*)m);
            generator_yield((void*)n);
            continue;
        }
        aux=(m<<1)+n;
        if(aux<=imax && br<2){
            aux = m;
            m=(m<<1)+n;
            n=aux;
            br=0;
            generator_yield((void*)m);
            generator_yield((void*)n);
            continue;
        }
        aux=m+(n<<1);
        if(aux<=imax && br<3){
            m=m+(n<<1);
            br=0;
            generator_yield((void*)m);
            generator_yield((void*)n);
            continue;
        }

        br=3;
        while(br==3){
            if((n<<1)>=m){
                aux=n;
                n=(n<<1)-m;
                m=aux;
                br=1;
            }else if(3*n>=m){
                aux=n;
                n=m-(n<<1);
                m=aux;
                br=2;
            }else{
                m-=n<<1;
                br=3;
            }
        }
        
        if(m==1 && br==1){
            m = 3;
            n = 1;
            br=0;
            generator_yield((void*)m);
            generator_yield((void*)n);
        }
    }
    return;
}

int main(void)
{
    generator_init();

    Generator *g = generator_create(coprimes);
    void *a, *b;
    while(!(g)->dead){
        a = generator_next(g, (void*)(5));
        b = generator_next(g, (void*)(5));
        if(!(g)->dead){
            printf("%ld , %ld\n", (long)a, (long)b);
        }
    }

    generator_destroy(g);

    return 0;
}

