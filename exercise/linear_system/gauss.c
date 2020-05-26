#include <stdio.h>
#include <stdlib.h>
#include "cmatrix.h"

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;

  int m, n;
  int i, j, k;
  double **a;
  double *b;

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

  /* perform Gauss elimination */
  for (k = 0; k < n; ++k) {
    for (i = k + 1; i < n; ++i) {
      for (j = k + 1; j < n; ++j) {
        mat_elem(a, i, j) -= mat_elem(a, k, j) * mat_elem(a, i, k) / mat_elem(a, k, k);
      }
      b[i] -= b[k] * mat_elem(a, i, k) / mat_elem(a, k, k);
    }
  }
  printf("Result of Gauss elimination (only upper triangular has meaning):\n");
  fprint_dmatrix(stdout, n, n, a);
  printf("Transformed vector B:\n");
  fprint_dvector(stdout, n, b);

  /* backward substitution */
  for (k = n-1; k >= 0; --k) {
    for (j = k + 1; j < n; ++j) {
      b[k] -= mat_elem(a, k, j) * b[j];
    }
    b[k] /= mat_elem(a, k, k);
  }
  printf("Solution X (transposed):\n");
  fprint_dvector(stdout, n, b);
  
  free_dmatrix(a);
  free_dvector(b);
}
