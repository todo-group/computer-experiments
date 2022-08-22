#include "zlange.h"
#include "cmatrix.h"
#include "mersenne_twister.h"
#include <stdio.h>

int main(void)
{
    const int seed = 12345;

    const char norm = 'F'; /* Frobenius norm */
    const int m = 5;
    const int n = 3;

    /* initialize random number generator */
    init_genrand(seed);

    /* generate random matrix */
    double complex** a = alloc_zmatrix(m, n);
    for (int j = 0; j < n; ++j)
    {
        for (int i = 0; i < m; ++i)
        {
            mat_elem(a, i, j) = CMPLX(genrand_real3(), genrand_real3());
        }
    }
    printf("Matrix A: ");
    fprint_zmatrix(stdout, m, n, a);

    double* work = (double*)NULL;
    /* working area (required only for the case with norm = 'I') */
    /* work = alloc_dvector(m); */

    /* calculate matrix norm */
    const double res = zlange_(&norm, &m, &n, mat_ptr(a), &m, vec_ptr(work));
    printf("||A|| = %lf\n", res);

    free_zmatrix(a);
    /* free_dvector(work); */
    return 0;
}
