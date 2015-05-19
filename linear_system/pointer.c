#include "matrix_util.h"
#include <stdio.h>

int main() {
  int n, i, j;
  double *v;
  double **m;
  n = 10;

  /* test for vector */
  v = alloc_dvector(n);
  for (i = 0; i < n; ++i) v[i] = i;
  fprint_dvector(stdout, n, v);
  
  printf("v     = %lu\n", (long)v);
  printf("&v[0] = %lu\n", (long)&v[0]);

  printf("(v+2) = %lu\n", (long)(v+2));
  printf("&v[2] = %lu\n", (long)&v[2]);

  printf("*v       = %10.5f\n", *v);
  printf("v[0]     = %10.5f\n", v[0]);

  printf("*(v+2)   = %10.5f\n", *(v+2));
  printf("v[2]     = %10.5f\n", v[2]);

  printf("(v+2)[3] = %10.5f\n", (v+2)[3]);
  printf("*(v+2+3) = %10.5f\n", *(v+2+3));

  free_dvector(v);
  
  /* test for matrix */
  m = alloc_dmatrix(n, n);
  for (i = 0; i < n; ++i)
    for (j = 0; j < n; ++j)
      m[i][j] = 100 * i + j;
  fprint_dmatrix(stdout, n, n, m);
  
  printf("m        = %lu\n", (long)m);
  printf("&m[0]    = %lu\n", (long)&m[0]);

  printf("m[0]     = %lu\n", (long)m[0]);
  printf("&m[0][0] = %lu\n", (long)&m[0][0]);

  printf("m[2]     = %lu\n", (long)m[2]);
  printf("&m[2][0] = %lu\n", (long)&m[2][0]);

  printf("m+2      = %lu\n", (long)(m+2));
  printf("&m[2]    = %lu\n", (long)&m[2]);

  printf("(*(m+2))[3] = %10.5f\n", (*(m+2))[3]);
  printf("*(*(m+2)+3) = %10.5f\n", *(*(m+2)+3));
  printf("m[2][3]     = %10.5f\n", m[2][3]);

  printf("*(m+2)[3]   = %10.5f\n", *(m+2)[3]);
  printf("*((m+2)[3]) = %10.5f\n", *((m+2)[3]));
  printf("*(m[5])     = %10.5f\n", *(m[5]));
  printf("m[5][0]     = %10.5f\n", m[5][0]);

  free_dmatrix(m);
}
