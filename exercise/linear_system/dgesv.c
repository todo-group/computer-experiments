#include <stdio.h>
#include <stdlib.h>
#include "cmatrix.h"
#include "dgesv.h"

int main() {
  int n = 3;

  double **a;
  double *b;

  int *ipiv;
  int info;
  int nrhs = 1;

  a = alloc_dmatrix(n, n);
  mat_elem(a, 0, 0) = 1;
  mat_elem(a, 0, 1) = 4;
  mat_elem(a, 0, 2) = 7;
  mat_elem(a, 1, 0) = 2;
  mat_elem(a, 1, 1) = 5;
  mat_elem(a, 1, 2) = 8;
  mat_elem(a, 2, 0) = 3;
  mat_elem(a, 2, 1) = 6;
  mat_elem(a, 2, 2) = 10;
  
  b = alloc_dvector(n);
  b[0] = 18;
  b[1] = 24;
  b[2] = 31;

  printf("Matrix A:\n");
  fprint_dmatrix(stdout, n, n, a);
  printf("Vector B (transposed):\n");
  fprint_dvector(stdout, n, b);

  /* solve linear equation */
  ipiv = alloc_ivector(n);
  dgesv_(&n, &nrhs, mat_ptr(a), &n, vec_ptr(ipiv), vec_ptr(b), &n, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgesv failed\n");
    exit(1);
  }

  printf("Solution X (transposed):\n");
  fprint_dvector(stdout, n, b);
  
  free_dmatrix(a);
  free_dvector(b);
  free_ivector(ipiv);
}
