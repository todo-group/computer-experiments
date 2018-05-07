#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>

/* http://www.netlib.org/lapack/explore-html/d3/d6a/dgetrf_8f.html */
extern void dgetrf_(int *M, int *N, double *A, int *LDA, int*IPIV, int *INFO);

/* http://www.netlib.org/lapack/explore-html/d6/d49/dgetrs_8f.html */
extern void dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, int *IPIV,
                    double *B, int *LDB, int *INFO);

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;

  int m, n;
  double **a;
  double *b;

  int *ipiv;
  int info;
  char trans = 'N';
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
  fprint_dmatrix(stdout, n, n, a);
  printf("Vector B (transposed):\n");
  fprint_dvector(stdout, n, b);

  /* perform LU decomposition */
  ipiv = alloc_ivector(n);
  dgetrf_(&n, &n, mat_ptr(a), &n, vec_ptr(ipiv), &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrf failed\n");
    exit(1);
  }
  printf("Result of LU decomposition:\n");
  fprint_dmatrix(stdout, n, n, a);
  printf("Pivot for LU decomposition:\n");
  fprint_ivector(stdout, n, ipiv);

  /* solve equations */
  dgetrs_(&trans, &n, &nrhs, mat_ptr(a), &n, vec_ptr(ipiv), vec_ptr(b), &n, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrs failed\n");
    exit(1);
  }
  printf("Solution X (transposed):\n");
  fprint_dvector(stdout, n, b);
  
  free_dmatrix(a);
  free_dvector(b);
  free_ivector(ipiv);
}
