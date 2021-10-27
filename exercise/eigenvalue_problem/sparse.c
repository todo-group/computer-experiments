/* Hamiltonian of one-dimensional transverse-field Ising model */

/* store only diagonal and non-zero offdiagonal elements */

#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>

/* calculate spin state at j-th site from bit pattern c */
double spin(int c, int j) { return 2.0 * ((c >> j) & 1) - 1; }

/* flip j-th spin */
int flip(int c, int j) { return c ^ (1 << j); }

/* fill diag[dim], od_num[dim], od_index[dim][nmax], od_value[dim][nmax] */
void set_matrix(int n, int nmax, double J, double h, double g, double *diag, int *od_num, int **od_index, double **od_value) {
  int dim = (1 << n);
  int c, i, j;
  double s0, s1;
  for (c = 0; c < dim; ++c) {
    diag[c] = 0;
    od_num[c] = 0;
  }
  for (c = 0; c < dim; ++c) {
    for (i = 0; i < n; ++i) {
      j = (i + 1) % n; /* periodic boundary condition */
      s0 = spin(c, i);
      s1 = spin(c, j);
      diag[c] += -J * s0 * s1 - h * s0;
    }
    for (i = 0; i < n; ++i) {
      mat_elem(od_index, c, od_num[c]) = flip(c, i);
      mat_elem(od_value, c, od_num[c]) = -g;
      od_num[c] += 1;
    }
  }
}

void multiply(int dim, double *diag, int *od_num, int **od_index, double **od_value, double *v, double *w) {
  int c, i;
  for (c = 0; c < dim; ++c) {
    w[c] = diag[c] * v[c];
    for (i = 0; i < od_num[c]; ++i)
      w[c] += mat_elem(od_value, c, i) * v[mat_elem(od_index, c, i)];
  }
}

int main(int argc, char** argv) {
  int n; /* number of spins */
  int dim; /* dimension of Hamiltonian */
  int nmax; /* maxmum number of non-zero offdiagonal elements for each row */
  double J, h, g;
  double *diag, **od_value;
  int *od_num, **od_index;
  int i, j;
  double *v, *w;

  n = 2; /* default value */
  J = 1;
  h = 0; /* longitudial field */
  g = 0; /* transverse field */
  if (argc > 1) n = atoi(argv[1]);
  if (argc > 2) h = atof(argv[2]);
  if (argc > 3) g = atof(argv[3]);
  dim = 1 << n;
  nmax = n;
  printf("n = %d; dim = %d; J = %f; h = %f; g = %f\n", n, dim, J, h, g);

  diag = alloc_dvector(dim);
  od_num = alloc_ivector(dim);
  od_index = alloc_imatrix(dim, nmax);
  od_value = alloc_dmatrix(dim, nmax);
  set_matrix(n, nmax, J, h, g, diag, od_num, od_index, od_value);

  printf("calculate product of Hamiltonian and identity matrix:\n");
  v = alloc_dvector(dim);
  w = alloc_dvector(dim);
  for (i = 0; i < dim; ++i) {
    for (j = 0; j < dim; ++j) v[j] = 0;
    v[i] = 1;
    multiply(dim, diag, od_num, od_index, od_value, v, w);
    fprint_dvector(stdout, dim, w);
  }
  free_dvector(v);
  free_dvector(w);

  free_dvector(diag);
  free_ivector(od_num);
  free_imatrix(od_index);
  free_dmatrix(od_value);
}
