#include <stdio.h>
#include <stdlib.h>

int main() {
  int n = 10;
  int i;
  double s;

  /* 長さ10の静的配列 (2.3.1節) */
  double v[10];

  /* 長さ10の動的配列 (2.12.1節) */
  double *w;
  w = (double*)malloc((size_t)(n * sizeof(double)));

  /* v[0]〜v[9]に1〜10を代入 (v[10]は存在しないことに注意) */
  for (i = 0; i < n; ++i) v[i] = i + 1;

  /* w[0]〜w[9] に1^2〜10^2を代入 (w[10]は存在しないことに注意) */
  for (i = 0; i < n; ++i) w[i] = (i + 1) * (i + 1);

  /* vとwを表示 */
  printf("v: ");
  for (i = 0; i < n; ++i) printf("%f ", v[i]);
  printf("\n");
  printf("w: ");
  for (i = 0; i < n; ++i) printf("%f ", w[i]);
  printf("\n");

  /* vの要素の和 */
  s = 0;
  for (i = 0; i < n; ++i) s += v[i];
  printf("sum of v[i] = %f\n", s);

  /* vとwの内積 */
  s = 0;
  for (i = 0; i < n; ++i) s += v[i] * w[i];
  printf("dot(v,w) = %f\n", s);

  /* 動的に確保したメモリ領域の解放 (2.12.2節) */
  free(w);
}
