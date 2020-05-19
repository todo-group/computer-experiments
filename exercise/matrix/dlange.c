#include <stdio.h>
#include "cmatrix.h"
#include "dlange.h"
#include "mersenne_twister.h"

int main(int argc, char** argv) {
  int seed = 12345;

  char norm = 'F'; /* Frobenius norm */
  int m = 5;
  int n = 3;

  int i, j;
  double **a;
  double *work = 0;
  double res;

  /* initialize random number generator */
  init_genrand(seed);

  /* generate random matrix */
  a = alloc_dmatrix(m, n);
  for (j = 0; j < n; ++j)
    for (i = 0; i < m; ++i)
      mat_elem(a, i, j) = genrand_real3();
  printf("Matrix A: ");
  fprint_dmatrix(stdout, m, n, a);

  /* working area (required only for the case with norm = 'I') */
  /* work = alloc_dvector(m); */

  /* calculate matrix norm */
  res = dlange_(&norm, &m, &n, mat_ptr(a), &m, vec_ptr(work));
  printf("||A|| = %lf\n", res);
  
  free_dmatrix(a);
  /* free_dvector(work); */
}
