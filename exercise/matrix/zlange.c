#include <stdio.h>
#include "cmatrix.h"
#include "zlange.h"
#include "mersenne_twister.h"

int main(int argc, char** argv) {
  int seed = 12345;

  char norm = 'F'; /* Frobenius norm */
  int m = 5;
  int n = 3;

  int i, j;
  double complex **a;
  double *work = 0;
  double res;

  /* initialize random number generator */
  init_genrand(seed);

  /* generate random matrix */
  a = alloc_zmatrix(m, n);
  for (j = 0; j < n; ++j)
    for (i = 0; i < m; ++i)
      mat_elem(a, i, j) = CMPLX(genrand_real3(), genrand_real3());
  printf("Matrix A: ");
  fprint_zmatrix(stdout, m, n, a);

  /* working area (required only for the case with norm = 'I') */
  /* work = alloc_dvector(m); */

  /* calculate matrix norm */
  res = zlange_(&norm, &m, &n, mat_ptr(a), &m, vec_ptr(work));
  printf("||A|| = %lf\n", res);
  
  free_zmatrix(a);
  /* free_dvector(work); */
}
