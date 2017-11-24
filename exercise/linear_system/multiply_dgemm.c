#include "matrix_util.h"
#include "mersenne_twister.h"
#include <stdio.h>

void dgemm_(char *TRANSA, char *TRANSB, int *M, int *N, int *K,
            double *ALPHA, double *A, int *LDA, double *B, int *LDB,
            double *BETA, double *C, int *LDC);
            
int main() {
  int n = 1000;
  int seed = 12345;
  double **matA, **matB, **matC;
  int i, j;
  char trans = 'N';
  double alpha = 1.0;
  double beta = 0.0;

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
  dgemm_(&trans, &trans, &n, &n, &n, &alpha, &matB[0][0], &n, &matA[0][0], &n,
         &beta, &matC[0][0], &n);

  printf("n = %d\n", n);
  printf("matC[0][0] = %15.10f\n", matC[0][0]);
  printf("matC[0][n-1] = %15.10f\n", matC[0][n-1]);
  printf("matC[n-1][n-1] = %15.10f\n", matC[n-1][n-1]);
}
