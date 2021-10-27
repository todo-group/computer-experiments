/* matrix representation of one-dimensional one-particle Hamiltonian */

/* store only diagonal and tridiagonal elements */

#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>

/* define potential V(x) for 0 < x < 1 */
double V(double x) {
  return 0.0; /* modify as you like */
}

/* fill diag[dim], lower[dim-1], upper[dim-1] */
void set_matrix(int dim, double *diag, double *lower, double *upper) {
  int n = dim + 1;
  double h = 1.0 / n;
  double hsinv = 1.0 / (h * h);
  int i;
  double x;
  for (i = 0; i < dim; ++i) {
    x = h * (i + 1);
    diag[i] = 2 * hsinv + V(x);
  }
  for (i = 0; i < (dim - 1); ++i) {
    lower[i] = -hsinv;
    upper[i] = -hsinv;
  }
}

void multiply(int dim, double *diag, double *lower, double *upper, double *v, double *w) {
  int i;
  w[0] = diag[0] * v[0] + upper[0] * v[1];
  for (i = 1; i < (dim - 1); ++i)
    w[i] = lower[i-1] * v[i-1] + diag[i] * v[i] + upper[i] * v[i+1];
  w[dim-1] = lower[dim-2] * v[dim-2] + diag[dim-1] * v[dim-1];
}

int main(int argc, char** argv) {
  int n; /* division */
  int dim; /* dimension of Hamiltonian */
  double *diag, *lower, *upper;
  int i, j;
  double *v, *w;

  n = 4; /* default value */
  if (argc > 1) n = atoi(argv[1]);
  dim = n - 1;
  printf("n = %d; dim = %d\n", n, dim);

  diag = alloc_dvector(dim);
  lower = alloc_dvector(dim-1);
  upper = alloc_dvector(dim-1);
  set_matrix(dim, diag, lower, upper);

  printf("calculate product of Hamiltonian and identity matrix:\n");
  v = alloc_dvector(dim);
  w = alloc_dvector(dim);
  for (i = 0; i < dim; ++i) {
    for (j = 0; j < dim; ++j) v[j] = 0;
    v[i] = 1;
    multiply(dim, diag, lower, upper, v, w);
    fprint_dvector(stdout, dim, w);
  }

  free_dvector(diag);
  free_dvector(lower);
  free_dvector(upper);
  free_dvector(v);
  free_dvector(w);
}
