/* Hamiltonian of one-dimensional transverse-field Ising model */

/* calculate elements on the fly */

#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>

/* calculate spin state at j-th site from bit pattern c */
double spin(int c, int j) { return 2.0 * ((c >> j) & 1) - 1; }

/* flip j-th spin */
int flip(int c, int j) { return c ^ (1 << j); }

void multiply(int n, double J, double h, double g, double *v, double *w) {
  int dim = (1 << n);
  int c, i, j;
  double s0, s1, diag;
  for (c = 0; c < dim; ++c) {
    diag = 0;
    for (i = 0; i < n; ++i) {
      j = (i + 1) % n; /* periodic boundary condition */
      s0 = spin(c, i);
      s1 = spin(c, j);
      diag += -J * s0 * s1 - h * s0;
    }
    w[c] = diag * v[c];
    for (i = 0; i < n; ++i) w[c] += -g * v[flip(c, i)];
  }
}

int main(int argc, char** argv) {
  int n; /* number of spins */
  int dim; /* dimension of Hamiltonian */
  double J, h, g;
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
  printf("n = %d; dim = %d; J = %f; h = %f; g = %f\n", n, dim, J, h, g);

  printf("calculate product of Hamiltonian and identity matrix:\n");
  v = alloc_dvector(dim);
  w = alloc_dvector(dim);
  for (i = 0; i < dim; ++i) {
    for (j = 0; j < dim; ++j) v[j] = 0;
    v[i] = 1;
    multiply(n, J, h, g, v, w);
    fprint_dvector(stdout, dim, w);
  }
  free_dvector(v);
  free_dvector(w);
}
