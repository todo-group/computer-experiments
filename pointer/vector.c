#include "matrix_util.h"
#include <stdio.h>

int main() {
  int m, i;
  double *v;
  m = 10;
  v = alloc_dvector(m);
  for (i = 0; i < m; ++i) v[i] = i;
  
  printf("v     = %lu\n", (long)v);
  printf("&v[0] = %lu\n", (long)&v[0]);

  printf("(v+2) = %lu\n", (long)(v+2));
  printf("&v[2] = %lu\n", (long)&v[2]);

  printf("*v       = %10.5f\n", *v);
  printf("v[0]     = %10.5f\n", v[0]);

  printf("*(v+2)   = %10.5f\n", *(v+2));
  printf("v[2]     = %10.5f\n", v[2]);

  printf("(v+2)[3] = %10.5f\n", (v+2)[3]);

  free_dvector(v);
}
