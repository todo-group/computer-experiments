#include "matrix_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* http://www.netlib. org/lapack/explore-html/d8/d2d/dgesvd_8f.html */
void dgesvd_(char *JOBU, char* JOBVT, int *M, int *N, double* A,
             int* LDA, double* S, double* U, int* LDU, double* VT, int* LDVT ,
             double* WORK, int* LWORK, int* INFO);

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;
  int i, j, h;

  int m, n, k;
  double **a, **u, **vt;
  double *s;
  int lwork;
  double *work;

  int info;
  char jobu = 'A';
  char jobvt = 'A';

  if (argc < 2) {
    fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
    exit(1);
  }
  filename = argv[1];

  /* read matrix A from a file */
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: file can not open\n");
    exit(1);
  }
  read_dmatrix(fp, &m, &n, &a);
  printf("Matrix A:\n");
  fprint_dmatrix(stdout, m, n, a);

  /* allocate matrices and vectors */
  if (m < n)
    k = m;
  else
    k = n;
  u = alloc_dmatrix(m, m);
  vt = alloc_dmatrix(n, n);
  s = alloc_dvector(k);
  lwork = 10 * k;
  work = alloc_dvector(lwork);
  
  /* perform SVD */
  dgesvd_(&jobu, &jobvt, &n, &m, &a[0][0], &n, &s[0], &vt[0][0], &n,
          &u[0][0], &m, &work[0], &lwork, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgesvd failed\n");
    exit(1);
  }
  printf("Result of SVD U:\n");
  fprint_dmatrix(stdout, m, m, u);
  printf("Result of SVD S:\n");
  fprint_dvector(stdout, k, s);
  printf("Result of SVD Vt:\n");
  fprint_dmatrix(stdout, n, n, vt);

  // check the result of SVD
  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) {
      a[i][j] = 0.0;
      for (h = 0; h < k; ++h) {
        a[i][j] += u[i][h] * s[h] * vt[h][j];
      }
    }
  }
  printf("Reconstruction of the original matrix A:\n");
  fprint_dmatrix(stdout, m, n, a);

  // approximate A by rank (k-1) matrix
  s[k-1] = 0.0; // set the last singular value to zero
  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) {
      a[i][j] = 0.0;
      for (h = 0; h < k; ++h) {
        a[i][j] += u[i][h] * s[h] * vt[h][j];
      }
    }
  }
  printf("Rank (k-1) approximation of A:\n");
  fprint_dmatrix(stdout, m, n, a);

  free_dmatrix(a);
  free_dmatrix(u);
  free_dmatrix(vt);
  free_dvector(s);
  free_dvector(work);
}
