#include "cmatrix.h"
#include "dgesvd.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int imin(int x, int y) { return (x < y) ? x : y; }
int imax(int x, int y) { return (x > y) ? x : y; }

int main(int argc, char** argv) {
  char* filename;
  FILE *fp;
  int i, j, k;

  int m, n, r;
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
  r = imin(m, n);
  u = alloc_dmatrix(m, m);
  vt = alloc_dmatrix(n, n);
  s = alloc_dvector(r);
  lwork = imax(3 * r + imax(m, n), 5 * r);
  work = alloc_dvector(lwork);
  
  /* perform SVD */
  dgesvd_(&jobu, &jobvt, &m, &n, mat_ptr(a), &m, vec_ptr(s), mat_ptr(u), &m,
          mat_ptr(vt), &n, vec_ptr(work), &lwork, &info);
  if (info != 0) {
    fprintf(stderr, "Error: LAPACK::dgesvd failed\n");
    exit(1);
  }
  printf("Result of SVD U:\n");
  fprint_dmatrix(stdout, m, m, u);
  printf("Result of SVD S:\n");
  fprint_dvector(stdout, r, s);
  printf("Result of SVD Vt:\n");
  fprint_dmatrix(stdout, n, n, vt);

  // check the result of SVD
  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) {
      mat_elem(a, i, j) = 0.0;
      for (k = 0; k < r; ++k) {
        mat_elem(a, i, j) += mat_elem(u, i, k) * s[k] * mat_elem(vt, k, j);
      }
    }
  }
  printf("Reconstruction of the original matrix A:\n");
  fprint_dmatrix(stdout, m, n, a);

  // approximate A by rank (r-1) matrix
  s[r-1] = 0.0; // set the last singular value to zero
  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) {
      mat_elem(a, i, j) = 0.0;
      for (k = 0; k < r; ++k) {
        mat_elem(a, i, j) += mat_elem(u, i, k) * s[k] * mat_elem(vt, k, j);
      }
    }
  }
  printf("Rank (r-1) approximation of A:\n");
  fprint_dmatrix(stdout, m, n, a);

  free_dmatrix(a);
  free_dmatrix(u);
  free_dmatrix(vt);
  free_dvector(s);
  free_dvector(work);
}
