#include <stdio.h>
#include <stdlib.h>
#include "cmatrix.h"
#include "dsyev.h"

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;

  int m, n;
  double **a; /* matrix */
  double *w; /* eigenvalues */
  double *work; /* working area */

  int lwork, info;
  char jobz = 'V'; /* Compute eigenvalues and eigenvectors */
  char uplo = 'U'; /* Upper triangle of A is stored */

  if (argc < 2) {
    fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
    exit(1);
  }
  filename = argv[1];

  /* read matrix A */
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: file can not open\n");
    exit(1);
  }
  read_dmatrix(fp, &m, &n, &a);
  if (m != n) {
    fprintf(stderr, "Error: matrix is not square\n");
    exit(1);
  }
  printf("Matrix A:\n");
  fprint_dmatrix(stdout, n, n, a);

  /* perform eigenvalue decomposition */
  w = alloc_dvector(n);
  lwork = 3*n - 1;
  work = alloc_dvector(lwork);
  dsyev_(&jobz, &uplo, &n, mat_ptr(a), &n, vec_ptr(w), vec_ptr(work), &lwork, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dsyev failed\n");
    exit(1);
  }
  printf("Eigenvalues:\n");
  fprint_dvector(stdout, n, w);
  printf("Eigenvectors [each column represents each eigenvector]:\n");
  fprint_dmatrix(stdout, n, n, a);

  free_dmatrix(a);
  free_dvector(w);
}
