#include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>

/* http://www.netlib.org/lapack/explore-html/d3/d6a/dgetrf_8f.html */
void dgetrf_(int *M, int *N, double *A, int *LDA, int*IPIV, int *INFO);

/* http://www.netlib.org/lapack/explore-html/d6/d49/dgetrs_8f.html */
void dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, int *IPIV,
             double *B, int *LDB, int *INFO);

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;

  int m, n;
  double **a;
  double *b;

  int *ipiv;
  int info;
  char trans = 'T';
  int nrhs = 1;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
    exit(1);
  }
  filename = argv[1];

  /* read matrix A and vector B from a file */
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: file can not open\n");
    exit(1);
  }
  read_dmatrix(fp, &m, &n, &a);
  if (m != n) {
    fprintf(stderr, "Error: inconsistent number of equations\n");
    exit(1);
  }
  read_dvector(fp, &n, &b);
  if (m != n) {
    fprintf(stderr, "Error: inconsistent number of equations\n");
    exit(1);
  }
  printf("Matrix A:\n");
  print_dmatrix(n, n, a);
  printf("Vector B (transposed):\n");
  print_dvector(n, b);

  /* perform LU decomposition */
  ipiv = alloc_ivector(n);
  dgetrf_(&n, &n, &a[0][0], &n, &ipiv[0], &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrf failed\n");
    exit(1);
  }
  printf("Result of LU decomposition:\n");
  print_dmatrix(n, n, a);
  printf("Pivot for LU decomposition:\n");
  print_ivector(n, ipiv);

  /* solve equations */
  dgetrs_(&trans, &n, &nrhs, &a[0][0], &n, &ipiv[0], &b[0], &n, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrs failed\n");
    exit(1);
  }
  printf("Solution X (transposed):\n");
  print_dvector(n, b);
  
  free_dmatrix(a);
  free_dvector(b);
  free_ivector(ipiv);
}
