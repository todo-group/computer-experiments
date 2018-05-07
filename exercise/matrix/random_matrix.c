#include "cmatrix.h"
#include "mersenne_twister.h"
#include <stdio.h>

int main() {
  int m, n;
  int i, j;
  double **mat;
  double *v;

  m = 100;
  n = 100;

  /* 乱数(Mersenne Twister)の初期化 */
  init_genrand(12345);

  /* 要素が [-1,1] のランダム行列の生成 */
  mat = alloc_dmatrix(m, n);
  for (j = 0; j < n; ++j) {
    for (i = 0; i < m; ++i) {
      mat_elem(mat, i, j) = 2.0 * genrand_real1() - 1.0;
    }
  }
  fprint_dmatrix(stdout, m, n, mat);
  free_dmatrix(mat);

  /* 要素が [-1,1] のランダムベクトルの生成 */
  v = alloc_dvector(n);
  for (i = 0; i < n; ++i) {
    v[i] = 2.0 * genrand_real1() - 1.0;
  }
  fprint_dvector(stdout, n, v);
  free_dvector(v);
}
