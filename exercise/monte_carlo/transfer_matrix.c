#include <stdio.h>
#include <math.h>
#include "matrix_util.h"

/* v = D^{1/2} v */
void product_d(int M, double temperature, double *v) {
  int N = 1 << M; /* N = 2^M */
  int i, j, s;
  double elem, w[2];
  w[0] = exp(0.5 / temperature);
  w[1] = exp(-0.0 / temperature);
  for (s = 0; s < N; ++s) {
    elem = 1.0;
    for (i = 0; i < M; ++i) {
      j = (i+1) % M;
      elem *= w[((s >> i) & 1) ^ ((s >> j) & 1)];
    }
    v[s] *= elem;
  }
}

/* v = U v */
void product_u(int M, double temperature, double *v, double *vtmp) {
  int N = 1 << M; /* N = 2^M */
  int i, s;
  double w[2];
  w[0] = exp(1.0 / temperature);
  w[1] = exp(-1.0 / temperature);
  for (i = 0; i < M; ++i) {
    for (s = 0; s < N; ++s)
      vtmp[s] = w[0] * v[s] + w[1] * v[s ^ (1 << i)];
    for (s = 0; s < N; ++s)
      v[s] = vtmp[s];
  }
}

void generate_t(int M, double temperature, double **T, double *v0, double *v1) {
  int N = 1 << M; /* N = 2^M */
  int i, j;
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) v0[j] = 0.0;
    v0[i] = 1.0;
    product_d(M, temperature, v0);
    product_u(M, temperature, v0);
    product_d(M, temperature, v0);
    for (j = 0; j < N; ++j) T[i][j] = v0[j];
  }
}
  
int main(int argc, char** argv) {
  double temperature = 2.0;
  int L = 4;
  int M = 4;
  int num_sites = L * M;
  int dim = 1 << M;
  double **T;
  double **TL;
  double *v0;
  double *v1;
  int i, j;
  T = allocate_dmatrix(dim, dim);
  TL = allocate_dmatrix(dim, dim);
  v0 = allocate_dvector(dim);
  v1 = allocate_dvector(dim);

  /* prepare transfer matrix T */
  generate_t(M, temperature, T, v0, v1);

  /* calculate T^L */
  for (int i = 0; i < L; ++i) {
  }
}
