#include <stdio.h>
#include <stdlib.h>
#include "matrix_util.h"
#include "poisson.h"

/* http://www.netlib.org/lapack/explore-html/d3/d6a/dgetrf_8f.html */
extern void dgetrf_(int *M, int *N, double *A, int *LDA, int*IPIV, int *INFO);

/* http://www.netlib.org/lapack/explore-html/d6/d49/dgetrs_8f.html */
extern void dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, int *IPIV,
                    double *B, int *LDB, int *INFO);

int main(int argc, char** argv) {
  int n, dim, x, y;
  double **a;
  double *b;

  int *ipiv;
  int info;
  char trans = 'T';
  int nrhs = 1;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s n\n", argv[0]);
    exit(1);
  }
  n = atoi(argv[1]);

  dim = matrix_dimension(n);
  a = alloc_dmatrix(dim, dim);
  generate_dense(n, 1.0/n, a);
  b = alloc_dvector(dim);
  generate_rhs(n, 1.0/n, b);

  /* perform LU decomposition */
  ipiv = alloc_ivector(dim);
  dgetrf_(&dim, &dim, &a[0][0], &dim, &ipiv[0], &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrf failed\n");
    exit(1);
  }

  /* solve equations */
  dgetrs_(&trans, &dim, &nrhs, &a[0][0], &dim, &ipiv[0], &b[0], &dim, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrs failed\n");
    exit(1);
  }
  printf("# Solution: x y u(x,y)\n");
  for (x = 0; x < n + 1; ++x) {
    for (y = 0; y < n + 1; ++y) {
      if (x == 0 || x == n || y == 0 || y == n) {
        printf("%d %d %15.10e\n", x, y, boundary(n, x, y));
      } else {
        printf("%d %d %15.10e\n", x, y, b[(n-1) * (y-1) + (x-1)]);
      }
    }
  }
  
  free_dmatrix(a);
  free_dvector(b);
  free_ivector(ipiv);
}
