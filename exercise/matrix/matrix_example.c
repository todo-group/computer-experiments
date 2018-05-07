#include "cmatrix.h"
#include <stdio.h>

int main() {
  int m, n;
  double *v;
  double **mat;
  FILE *fp;

  m = 3;
  n = 5;

  /*
   * 倍精度行列
   */

  /* メモリ領域を動的に確保 */
  mat = alloc_dmatrix(m, n);

  /* 行列を開放 */
  free_dmatrix(mat);

  /* 行列をファイルから読み込み (同時にメモリが確保されることに注意) */
  fp = fopen("matrix.dat", "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: file can not be opened\n");
    exit(1);
  }
  read_dmatrix(fp, &m, &n, &mat);
  fclose(fp);

  /* 行列を出力 */
  fprint_dmatrix(stdout, m, n, mat);

  /* 行列を開放 */
  free_dmatrix(mat);

  /*
   * 倍精度ベクトル
   */

  /* メモリ領域を動的に確保 */
  v = alloc_dvector(n);

  /* ベクトルを開放 */
  free_dvector(v);

  /* ベクトルをファイルから読み込み (同時にメモリが確保されることに注意) */
  fp = fopen("vector.dat", "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: file can not be opened\n");
    exit(1);
  }
  read_dvector(fp, &n, &v);
  fclose(fp);

  /* ベクトルを出力 */
  fprint_dvector(stdout, n, v);

  /* ベクトルを開放 */
  free_dvector(v);
}
