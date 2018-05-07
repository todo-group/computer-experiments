#include "cmatrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* calculate r = b - A x */
void residual(int n, double **a, double *b, double *x, double *r) {
  int i, j;
  for (i = 0; i < n; ++i) {
    r[i] = b[i];
    for (j = 0; j < n; ++j) {
      r[i] -= mat_elem(a, i, j) * x[j];
    }
  }
}

/* calculate x^t y */
double inner(int n, double *x, double *y) {
  int i;
  double r = 0.0;
  for (i = 0; i < n; ++i) {
    r += x[i] * y[i];
  }
  return r;
}

/* calculate x^t A x */
double quad(int n, double **a, double *x) {
  int i, j;
  double r = 0.0;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      r += x[i] * mat_elem(a, i, j) * x[j];
    }
  }
  return r;
}


int main(int argc, char** argv) {
  char* filename;
  FILE *fp;

  int m, n, i, j, k;
  double **a;
  double *b, *x, *r;
  double norm2, alpha;

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

  /* initial condition */
  x = alloc_dvector(n);
  for (i = 0; i < n; ++i) {
    x[i] = 0.0;
  }
  
  /* steepest descent iteration */
  r = alloc_dvector(n);
  residual(n, a, b, x, r);
  for (k = 0; k < 1000; ++k) {
    alpha = inner(n, r, r) / quad(n, a, r);
    for (i = 0; i < n; ++i) {
      x[i] += alpha * r[i];
    }
    residual(n, a, b, x, r);
    norm2 = inner(n, r, r);
    printf("iteration %3d: residual = %10.5e\n", k, norm2);
    if (norm2 < 1e-20) break;
  }
  printf("Solution X:\n");
  fprint_dvector(stdout, n, x);
  
  free_dmatrix(a);
  free_dvector(b);
  free_dvector(x);
  free_dvector(r);
}
