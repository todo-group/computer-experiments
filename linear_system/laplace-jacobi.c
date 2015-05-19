/* Jacobi 法のサンプル
   収束判定を行っていないことに注意 */

#include "matrix_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
  int n;
  int i, j, iter;
  double **u;
  double **u_new;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s n\n", argv[0]);
    exit(1);
  }
  n = atoi(argv[1]);

  /* define vector u as two-dimensional matrix */
  u = alloc_dmatrix(n+1, n+1);
  for (i = 0; i < n+1; ++i)
    for (j = 0; j < n+1; ++j)
      u[i][j] = 0.0;
  u_new = alloc_dmatrix(n+1, n+1);
  /* NOTE: u_ij is given by u[i][j] */

  /* set boundary condition */
  for (j = 0; j <= n; ++j)
    u[0][j] = sin(M_PI *  j / n);
  
  for (iter = 0; iter < 100; ++iter) {
    for (i = 1; i < n; ++i)
      for (j = 1; j < n; ++j)
        u_new[i][j] = (u[i+1][j] + u[i-1][j] + u[i][j+1] + u[i][j-1]) / 4;
    for (i = 1; i < n; ++i)
      for (j = 1; j < n; ++j)
        u[i][j] = u_new[i][j];
  }
  
  printf("# Solution u_ij:\n");
  for (i = 0; i <= n; ++i) {
    for (j = 0; j <= n; ++j) {
      printf("%10.5f %10.5f %10.5f\n", 1.0 * i / n, 1.0 * j / n, u[i][j]);
    }
    printf("\n");
  }
  
  free_dmatrix(u);
  free_dmatrix(u_new);
}
