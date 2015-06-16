#include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* http://www.netlib.org/lapack/explore-html/d3/d6a/dgetrf_8f.html */
void dgetrf_(int *M, int *N, double *A, int *LDA, int*IPIV, int *INFO);

/* http://www.netlib.org/lapack/explore-html/d6/d49/dgetrs_8f.html */
void dgetrs_(char *TRANS, int *N, int *NRHS, double *A, int *LDA, int *IPIV,
             double *B, int *LDB, int *INFO);

 /* Maximum number of data points */
const int maxdata = 100;

 /* Number of Basis Functions */
int nbase = 2;

/* Basis Functions f(i, x) = Phi_i(x) */
double f(int i, double x) {
  if (i == 0)
    return 1.0;
  else if (i == 1)
    return x;
  else if (i == 2)
    return x * x;
  return 0;
}

void read_data(FILE *fp, int *n, double *x, double *y, double *yerror) {
  *n = 0;
  double xin, yin, rin;
  while (fscanf(fp, "%lf %lf %lf", &xin, &yin, &rin) == 3) {
    x[*n] = xin;
    y[*n] = yin;
    yerror[*n] = rin;
    ++(*n);
  }
}

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;
  int i, j, k;
  int n;
  double x[maxdata], y[maxdata], yerror[maxdata];
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

  /* read data from a file */
  fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: file can not open\n");
    exit(1);
  }
  read_data(fp, &n, x, y, yerror);
  printf("x:\n");
  fprint_dvector(stdout, n, x);
  printf("y:\n");
  fprint_dvector(stdout, n, y);
  printf("yerror:\n");
  fprint_dvector(stdout, n, yerror);

  /* construct a = Phi^t Phi */
  a = alloc_dmatrix(nbase, nbase);
  for (i = 0; i < nbase; ++i) {
    for (j = 0; j < nbase; ++j) {
      a[i][j] = 0.0;
      for (k = 0; k < n; ++k) {
        a[i][j] += f(i, x[k]) * f(j, x[k]);
      }
    }
  }
  printf("Phi^t Phi:\n");
  fprint_dmatrix(stdout, nbase, nbase, a);

  /* construct b = Phi^t y */
  b = alloc_dvector(nbase);
  for (i = 0; i < nbase; ++i) {
    b[i] = 0.0;
    for (k = 0; k < n; ++k) {
      b[i] += f(i, x[k]) * y[k];
    }
  }
  printf("Phi^t y:\n");
  fprint_dvector(stdout, nbase, b);

  /* perform LU decomposition */
  ipiv = alloc_ivector(nbase);
  dgetrf_(&nbase, &nbase, &a[0][0], &nbase, &ipiv[0], &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrf failed\n");
    exit(1);
  }
  printf("Result of LU decomposition:\n");
  fprint_dmatrix(stdout, nbase, nbase, a);
  printf("Pivot for LU decomposition:\n");
  fprint_ivector(stdout, nbase, ipiv);

  /* solve equations */
  dgetrs_(&trans, &nbase, &nrhs, &a[0][0], &nbase, &ipiv[0], &b[0], &nbase, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgetrs failed\n");
    exit(1);
  }
  printf("Result of linear regression w:\n");
  fprint_dvector(stdout, nbase, b);
  
  free_dmatrix(a);
  free_dvector(b);
}
