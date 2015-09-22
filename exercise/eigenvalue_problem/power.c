#include "matrix_util.h"
#include "mersenne_twister.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;

  int m, n;
  double **a; /* matrix */
  double *v, *w; /* working vectors */

  int i, j, k;
  double norm, eigval;
  
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

  /* prepare random initial vector */
  v = alloc_dvector(n);
  w = alloc_dvector(n);
  init_genrand(12345);
  for (i = 0; i < n; ++i) v[i] = 2.0 * genrand_real1() - 1.0;

  /* normalize initial vector */
  norm = 0.0;
  for (i = 0; i < n; ++i) norm += v[i] * v[i];
  norm = sqrt(norm);
  for (i = 0; i < n; ++i) v[i] /= norm;

  
  for (k = 0; k < 10; ++k) {
    /* matrix-vector multiplication */
    for (i = 0; i < n; ++i) {
      w[i] = 0.0;
      for (j = 0; j < n; ++j) {
        w[i] += a[i][j] * v[j];
      }
    }

    /* calculate eigenvalue */
    eigval = 0.0;
    /* ... */
    printf("iteration: %d  eigenvalue: %10.5f\n", k, eigval);
    
    /* normalize vector */
    norm = 0.0;
    for (i = 0; i < n; ++i) norm += w[i] * w[i];
    norm = sqrt(norm);
    for (i = 0; i < n; ++i) v[i] = w[i] / norm;
  }
  
  printf("Eigenvector:\n");
  fprint_dvector(stdout, n, v);

  free_dmatrix(a);
  free_dvector(v);
  free_dvector(w);
}
