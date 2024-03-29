#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /* allocate vector of length n */
    const int n = 10;
    double* v = (double*)malloc((size_t)(n * sizeof(double)));
    for (int i = 0; i < n; ++i)
    {
        v[i] = i;
    }

    printf("%d\n", n);
    for (int i = 0; i < n; ++i)
    {
        printf("%10.5f ", v[i]);
    }
    printf("\n");

    printf("sizeof(double) = %lu\n", sizeof(double));

    printf("v     = %lu\n", (long)v);
    printf("&v[0] = %lu\n", (long)&v[0]);

    printf("(v+2) = %lu\n", (long)(v + 2));
    printf("&v[2] = %lu\n", (long)&v[2]);

    printf("*v       = %10.5f\n", *v);
    printf("v[0]     = %10.5f\n", v[0]);

    printf("*(v+2)   = %10.5f\n", *(v + 2));
    printf("v[2]     = %10.5f\n", v[2]);

    printf("(v+2)[3] = %10.5f\n", (v + 2)[3]);
    printf("*(v+2+3) = %10.5f\n", *(v + 2 + 3));

    free(v);
    return 0;
}
