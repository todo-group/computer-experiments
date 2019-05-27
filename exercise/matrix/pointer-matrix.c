#include <stdio.h>
#include <stdlib.h>

int main() {
  int n, i, j;
  double **a;
  n = 10;

  a = (double**)malloc((size_t)(n * sizeof(double*)));
  for (int i = 0; i < n; ++i)
    a[i] = (double*)malloc((size_t)(n * sizeof(double)));

  for (i = 0; i < n; ++i)
    for (j = 0; j < n; ++j)
      a[i][j] = 100 * i + j;
  
  printf("a        = %lu\n", (long)a);
  printf("&a[0]    = %lu\n", (long)&a[0]);

  printf("a[0]     = %lu\n", (long)a[0]);
  printf("&a[0][0] = %lu\n", (long)&a[0][0]);

  printf("a[2]     = %lu\n", (long)a[2]);
  printf("&a[2][0] = %lu\n", (long)&a[2][0]);

  printf("a+2      = %lu\n", (long)(a+2));
  printf("&a[2]    = %lu\n", (long)&a[2]);

  printf("(*(a+2))[3] = %10.5f\n", (*(a+2))[3]);
  printf("*(*(a+2)+3) = %10.5f\n", *(*(a+2)+3));
  printf("a[2][3]     = %10.5f\n", a[2][3]);

  printf("*(a+2)[3]   = %10.5f\n", *(a+2)[3]);
  printf("*((a+2)[3]) = %10.5f\n", *((a+2)[3]));
  printf("*(a[5])     = %10.5f\n", *(a[5]));
  printf("a[5][0]     = %10.5f\n", a[5][0]);

  for (int i = 0; i < n; ++i) free(a[i]);
  free(a);
}
