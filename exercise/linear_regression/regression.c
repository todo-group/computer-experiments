#include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* http://www.netlib. org/lapack/explore-html/d8/d2d/dgesvd_8f.html */
void dgesvd_(char *JOBU, char* JOBVT, int *M, int *N, double* A,
             int* LDA, double* S, double* U, int* LDU, double* VT, int* LDVT ,
             double* WORK, int* LWORK, int* INFO);

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
  /* else if (i == 2) */
  /*   return x * x; */
  return 0;
}

int imin(int x, int y) { return (x < y) ? x : y; }
int imax(int x, int y) { return (x > y) ? x : y; }

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
  double **a, **u, **vt;
  double *b, *s, *w;
  int lwork;
  double *work;
  int info;
  char jobu = 'S';
  char jobvt = 'S';

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

  /* perform svd on a */
  u = alloc_dmatrix(nbase, nbase);
  vt = alloc_dmatrix(nbase, nbase);
  s = alloc_dvector(nbase);
  lwork = 5 * nbase;
  work = alloc_dvector(lwork);
  dgesvd_(&jobu, &jobvt, &nbase, &nbase, &a[0][0], &nbase, &s[0], &vt[0][0],
          &nbase, &u[0][0], &nbase, &work[0], &lwork, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgesvd failed\n");
    exit(1);
  }

  printf("Result of SVD U:\n");
  fprint_dmatrix(stdout, nbase, nbase, u);
  printf("Result of SVD S:\n");
  fprint_dvector(stdout, nbase, s);
  printf("Result of SVD Vt:\n");
  fprint_dmatrix(stdout, nbase, nbase, vt);

  // check the result of SVD
  for (i = 0; i < nbase; ++i) {
    for (j = 0; j < nbase; ++j) {
      a[i][j] = 0.0;
      for (k = 0; k < nbase; ++k) {
        a[i][j] += u[i][k] * s[k] * vt[k][j];
      }
    }
  }
  printf("Reconstruction of the original matrix A:\n");
  fprint_dmatrix(stdout, nbase, nbase, a);

  /* invserse s */
  for (i = 0; i < nbase; ++i) {
    if (s[i] > 1.0e-10) s[i] = 1.0 / s[i];
  }

  w = alloc_dvector(nbase);
  for (i = 0; i < nbase; ++i) {
    w[i] = 0.0;
    for (j = 0; j < nbase; ++j) {
      for (k = 0; k < nbase; ++k) {
        w[i] += vt[j][i] * s[j] * u[k][j] * b[k];
      }
    }
  }
  printf("Result of linear regression w:\n");
  fprint_dvector(stdout, nbase, w);
  
  free_dmatrix(a);
  free_dmatrix(u);
  free_dmatrix(vt);
  free_dvector(b);
  free_dvector(s);
  free_dvector(w);
  free_dvector(work);
}
