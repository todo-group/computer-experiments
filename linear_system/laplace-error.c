#include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* http://www.netlib.org/lapack/explore-html/d3/d6a/dgetrf_8f.html */
void dgetrf_(int *M, int *N, double *A, int *LDA, int*IPIV, int *INFO);

/* http://www.netlib.org/lapack/explore-html/d6/d49/dgetrs_8f.html */
void dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, int *IPIV,
             double *B, int *LDB, int *INFO);

int main(int argc, char** argv) {
  int n, dim;
  int i, j;
  double h;
  double **a;
  double *u;

  int *ipiv;
  int info;
  char trans = 'T';
  int nrhs = 1;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s n\n", argv[0]);
    exit(1);
  }
  n = atoi(argv[1]);
  dim = (n+1) * (n+1);
  h = 1.0 / n;

  /* A is (n+1)^2 x (n+1)^2 matrix (NOT (n+1) x (n+1)) */
  a = alloc_dmatrix(dim, dim);
  u = alloc_dvector(dim);
  /* NOTE: u_ij is given by u[(n+1) * i + j] */

  /* set boundary condition */
  for (i = 0; i <= n; ++i) {
    a[(n+1) * i + 0][(n+1) * i + 0] = 1.0;
    u[(n+1) * i + 0] = 0.0;
    a[(n+1) * i + n][(n+1) * i + n] = 1.0;
    u[(n+1) * i + n] = 0.0;
  }
  for (j = 0; j <= n; ++j) {
    a[(n+1) * 0 + j][(n+1) * 0 + j] = 1.0;
    u[(n+1) * 0 + j] = sin(M_PI *  j / n);
    a[(n+1) * n + j][(n+1) * n + j] = 1.0;
    u[(n+1) * n + j] = 0.0;
  }
  
  /* set coefficients */
  for (i = 1; i < n; ++i) {
    for (j = 1; j < n; ++j) {
      a[(n+1) * i + j][(n+1) * (i+1) + j] = 1.0 / (h * h);
      a[(n+1) * i + j][(n+1) * i + j] = -2.0 / (h * h);
      a[(n+1) * i + j][(n+1) * (i-1) + j] = 1.0 / (h * h);
      a[(n+1) * i + j][(n+1) * i + (j+1)] = 1.0 / (h * h);
      a[(n+1) * i + j][(n+1) * i + j] = -2.0 / (h * h);
      a[(n+1) * i + j][(n+1) * i + (j-1)] = 1.0 / (h * h);
      u[(n+1) * i + j] = 0.0;
    }
  }
  
  /* perform LU decomposition */
  ipiv = alloc_ivector(dim);
  dgetrf_(&dim, &dim, &a[0][0], &dim, &ipiv[0], &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrf failed\n");
    exit(1);
  }

  /* solve equations */
  dgetrs_(&trans, &dim, &nrhs, &a[0][0], &dim, &ipiv[0], &u[0], &dim, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrs failed\n");
    exit(1);
  }

  printf("# Solution u_ij:\n");
  for (i = 0; i <= n; ++i)
    for (j = 0; j <= n; ++j)
      printf("%10.5f %10.5f %10.5f\n", 1.0 * i / n, 1.0 * j / n, u[(n+1) * i + j]);
  
  free_dmatrix(a);
  free_dvector(u);
  free_ivector(ipiv);
}
