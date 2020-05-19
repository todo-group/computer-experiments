#include <stdio.h>
#include "cmatrix.h"
#include "mersenne_twister.h"

int main() {
  int n = 1000;
  int seed = 12345;
  double **matA, **matB, **matC;
  int i, j, k;

  /* initialize random number generator */
  init_genrand(seed);
  
  /* generate random matrices */
  matA = alloc_dmatrix(n, n);
  matB = alloc_dmatrix(n, n);
  for (j = 0; j < n; ++j) {
    for (i = 0; i < n; ++i) {
      mat_elem(matA, i, j) = genrand_real3();
      mat_elem(matB, i, j) = genrand_real3();
    }
  }

  /* calculate C = A * B */
  matC = alloc_dmatrix(n, n);
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      mat_elem(matC, i, j) = 0.0;
      for (k = 0; k < n; ++k) {
        mat_elem(matC, i, j) += mat_elem(matA, i, k) * mat_elem(matB, k, j);
      }
    }
  }

  printf("n = %d\n", n);
  printf("matC[0][0] = %15.10f\n", mat_elem(matC, 0, 0));
  printf("matC[0][n-1] = %15.10f\n", mat_elem(matC, 0, n-1));
  printf("matC[n-1][n-1] = %15.10f\n", mat_elem(matC, n-1, n-1));
}
