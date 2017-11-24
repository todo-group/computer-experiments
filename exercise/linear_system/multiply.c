#include "matrix_util.h"
#include "mersenne_twister.h"
#include <stdio.h>

int main() {
  int n = 1000;
  int seed = 12345;
  double **matA, **matB, **matC;
  int i, j, k;

  /* generate random matrices */
  matA = alloc_dmatrix(n, n);
  matB = alloc_dmatrix(n, n);
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      matA[i][j] = genrand_real3();
      matB[i][j] = genrand_real3();
    }
  }

  /* calculate C = A * B */
  matC = alloc_dmatrix(n, n);
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      matC[i][j] = 0.0;
      for (k = 0; k < n; ++k) {
        matC[i][j] += matA[i][k] * matB[k][j];
      }
    }
  }

  printf("n = %d\n", n);
  printf("matC[0][0] = %15.10f\n", matC[0][0]);
  printf("matC[0][n-1] = %15.10f\n", matC[0][n-1]);
  printf("matC[n-1][n-1] = %15.10f\n", matC[n-1][n-1]);
}
