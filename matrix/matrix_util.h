/* Utility for vector and matrix allocation/deallocation */

/* List of functions

  alloc_dvector : allocate vector of double
  alloc_fvector : allocate vector of float
  alloc_ivector : allocate vector of int

  alloc_dmatrix : allocate matrix of double
  alloc_fmatrix : allocate matrix of float

  free_dvector : deallocate vector of double
  free_fvector : deallocate vector of float
  free_ivector : deallocate vector of int

  free_dmatrix : deallocate matrix of double
  free_fmatrix : deallocate matrix of float

  print_dvector: print out vector of double
  print_fvector: print out vector of float
  print_ivector: print out vector of int

  print_dmatrix: print out matrix of double
  print_fmatrix: print out matrix of float

  read_dvector: read vector of double from file
  read_fvector: read vector of float from file
  read_ivector: read vector of int from file

  read_dmatrix: read matrix of double from file
  read_fmatrix: read matrix of float from file
*/

#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H

#include <stdlib.h>
#include <stdio.h>

/* allocate vector of double */
static inline double *alloc_dvector(int n) {
  double *vec;
  vec = (double*)malloc((size_t)(n * sizeof(double)));
  if (vec == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_dvector\n");
    exit(1);
  }
  return vec;
}

/* allocate vector of float */
static inline float *alloc_fvector(int n) {
  float *vec;
  vec = (float*)malloc((size_t)(n * sizeof(float)));
  if (vec == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_fvector\n");
    exit(1);
  }
  return vec;
}

/* allocate vector of int */
static inline int *alloc_ivector(int n) {
  int *vec;
  vec = (int*)malloc((size_t)(n * sizeof(int)));
  if (vec == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_ivector\n");
    exit(1);
  }
  return vec;
}

/* allocate m x n matrix of double */
static inline double **alloc_dmatrix(int m, int n) {
  int i;
  double **mat;
  mat = (double**)malloc((size_t)(m * sizeof(double*)));
  if (mat == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_dmatrix\n");
    exit(1);
  }
  mat[0] = (double*)malloc((size_t)(m * n * sizeof(double)));
  if (mat[0] == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_dmatrix\n");
    exit(1);
  }
  for (i = 1; i < m; ++i) mat[i] = mat[i-1] + n;
  return mat;
}

/* allocate m x n matrix of float */
static inline float **alloc_fmatrix(int m, int n) {
  int i;
  float **mat;
  mat = (float**)malloc((size_t)(m * sizeof(float*)));
  if (mat == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_fmatrix\n");
    exit(1);
  }
  mat[0] = (float*)malloc((size_t)(m * n * sizeof(float)));
  if (mat[0] == NULL) {
    fprintf(stderr, "Error: allocation failed in alloc_fmatrix\n");
    exit(1);
  }
  for (i = 1; i < m; ++i) mat[i] = mat[i-1] + n;
  return mat;
}

/* deallocate vector of double */
static inline void free_dvector(double *vec) {
  free(vec);
}

/* deallocate vector of float */
static inline void free_fvector(float *vec) {
  free(vec);
}

/* deallocate vector of int */
static inline void free_ivector(int *vec) {
  free(vec);
}

/* deallocate matrix of double */
static inline void free_dmatrix(double **mat) {
  free(mat[0]);
  free(mat);
}

/* deallocate float matrix of float */
static inline void free_fmatrix(float **mat) {
  free(mat[0]);
  free(mat);
}

/* print out vector of double */
static inline void print_dvector(int n, double *vec) {
  int i;
  printf("%d\n", n);
  for (i = 0; i < n; ++i) printf("%10.5f ", vec[i]);
  printf("\n");
}

/* print out vector of float */
static inline void print_fvector(int n, float *vec) {
  int i;
  printf("%d\n", n);
  for (i = 0; i < n; ++i) printf("%10.5f ", vec[i]);
  printf("\n");
}

/* print out vector of int */
static inline void print_ivector(int n, int *vec) {
  int i;
  printf("%d\n", n);
  for (i = 0; i < n; ++i) printf("%d ", vec[i]);
  printf("\n");
}

/* print out matrix of double */
static inline void print_dmatrix(int m, int n, double **mat) {
  int i, j;
  printf("%d %d\n", m, n);
  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) printf("%10.5f ", mat[i][j]);
    printf("\n");
  }
}

/* print out matrix of float */
static inline void print_fmatrix(int m, int n, float **mat) {
  int i, j;
  printf("%d %d\n", m, n);
  for (i = 0; i < m; ++i) {
    for (j = 0; j < n; ++j) printf("%10.5f ", mat[i][j]);
    printf("\n");
  }
}

/* read vector of double from file */
static inline void read_dvector(FILE *fp, int *n, double **vec) {
  int i;
  fscanf(fp, "%d", n);
  *vec = alloc_dvector(*n);
  for (i = 0; i < *n; ++i) fscanf(fp, "%lf", &(*vec)[i]);
}

/* read vector of float from file */
static inline void read_fvector(FILE *fp, int *n, float **vec) {
  int i;
  fscanf(fp, "%d", n);
  *vec = alloc_fvector(*n);
  for (i = 0; i < *n; ++i) fscanf(fp, "%f", &(*vec)[i]);
}

/* read matrix of double from file */
static inline void read_dmatrix(FILE *fp, int *m, int *n, double ***mat) {
  int i, j;
  fscanf(fp, "%d", m);
  fscanf(fp, "%d", n);
  *mat = alloc_dmatrix(*m, *n);
  for (i = 0; i < *m; ++i) {
    for (j = 0; j < *n; ++j) fscanf(fp, "%lf", &(*mat)[i][j]);
  }
}

/* read matrix of float from file */
static inline void read_fmatrix(FILE *fp, int *m, int *n, float ***mat) {
  int i, j;
  fscanf(fp, "%d", m);
  fscanf(fp, "%d", n);
  *mat = alloc_fmatrix(*m, *n);
  for (i = 0; i < *m; ++i) {
    for (j = 0; j < *n; ++j) fscanf(fp, "%f", &(*mat)[i][j]);
  }
}

#endif
