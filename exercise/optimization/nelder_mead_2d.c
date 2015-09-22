#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double f(const double *x) {
  double r2 = (x[0]*x[0] + x[1]*x[1]);
  return -10*r2 + r2*r2 - 2*(x[0]+x[1]);
}

/* v0 と v1 の値を交換する関数 */
void swap(double* v0, double *v1) {
  double vt;
  vt = *v0;
  *v0 = *v1;
  *v1 = vt;
}

int main(int argc, char** argv) {
  const int n = 2; /* 次元 (simplexの頂点数はn+1) */
  double x[n+1][n];
  double y[n+1];
  int i, j;
  double xnew[n], xg[n], diff;

  /* 初期値の設定 (3点が一直線上にのらないように選ぶ) */
  x[0][0] = -15.0;
  x[0][1] = 3.0;
  x[1][0] = x[0][0] + 0.1;
  x[1][1] = x[0][1];
  x[2][0] = x[0][0];
  x[2][1] = x[0][1] + 0.1;

  for (;;) {
    /* 関数値の評価 */
    for (i = 0; i <= n; ++i) y[i] = f(x[i]);

    /* y[n] が最大値、y[n-1]が二番目大きな値、y[0] が最小値となるように並び替える */
    for (i = 0; i < n; ++i) {
      if (y[i] > y[n]) {
        for (j = 0; j < n; ++j) swap(&x[i][j], &x[n][j]);
        swap(&y[i], &y[n]);
      }
    }
    for (i = 1; i < n; ++i) {
      if (y[i] < y[0]) {
        for (j = 0; j < n; ++j) swap(&x[i][j], &x[0][j]);
        swap(&y[i], &y[0]);
      }
    }
    for (i = 1; i < n-1; ++i) {
      if (y[i] > y[n-1]) {
        for (j = 0; j < n; ++j) swap(&x[i][j], &x[n-1][j]);
        swap(&y[i], &y[n-1]);
      }
    }

    for (i = 0; i <= n; ++i) {
      printf("(x[%d],y[%d]) = (%15.10f,%15.10f) %15.10f\n",
             i, i, x[i][0], x[i][1], y[i]);
    }

    /* 収束判定 */
    diff = 0.0;
    for (j = 0; j < n; ++j) diff += (x[n][j] - x[0][j]) * (x[n][j] - x[0][j]);
    diff = sqrt(diff);
    if (diff < 1.0e-6 && fabs(y[n] - y[0]) < 1.0e-12) {
      break;
    }

    /* 最大値をのぞいた点の重心 */
    
    /* 反射: x[n]をxgに関する対称な点に移動 */

    /* 拡大: 反射点がy[0]よりも小さくなるようであればさらに先に進む */

    /* 縮小: 二番目に悪い値よりもまだ大きい場合にはxgに近づける */

    /* 収縮: それでもよくならない場合にはx[0]以外の点をx[0]に近づける */
  }
}
