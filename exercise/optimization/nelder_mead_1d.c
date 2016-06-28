#include "func_1d.h"
#include <stdio.h>

/* v0 と v1 の値を交換する関数 */
void swap(double* v0, double *v1) {
  double vt;
  vt = *v0;
  *v0 = *v1;
  *v1 = vt;
}

int main(int argc, char** argv) {
  const int n = 1; /* 次元 (simplexの頂点数はn+1) */
  double x[n+1];
  double y[n+1];
  int i;
  double xnew, xg;

  /* 初期値の設定 */
  x[0] = 3.0;
  /* x[0] = atof(argv[1]); */
  x[1] = x[0] + 0.1;

  for (;;) {
    /* 関数値の評価 */
    for (i = 0; i <= n; ++i) y[i] = f(x[i]);
    
    /* y[0] が最小値、y[n] が最大値となるように並び替える */
    if (y[0] > y[n]) {
      swap(&x[0], &x[n]);
      swap(&y[0], &y[n]);
    }

    for (i = 0; i <= n; ++i) {
      printf("(x[%d],y[%d]) = %15.10f %15.10f\n", i, i, x[i], y[i]);
    }

    /* 収束判定 */
    if (fabs(x[n] - x[0]) < 1.0e-6 && fabs(y[n] - y[0]) < 1.0e-12) {
      break;
    }

    /* 最大値をのぞいた点の重心 (一次元では x[0] そのもの) */
    xg = x[0];
    
    /* 反射: x[n]をxgに関する対称な点に移動 */
    xnew = xg - (x[n] - xg);
    if (f(xnew) < y[n]) x[n] = xnew;
    if (f(xnew) < y[0]) {
      /* 拡大: 反射点がy[0]よりも小さくなるようであればさらに先に進む */
      xnew = xg + 2 * (x[n] - xg);
      if (f(xnew) < y[0]) x[n] = xnew;
    } else if (f(xnew) > y[n-1]) {
      /* 縮小: 二番目に悪い値よりもまだ大きい場合にはxgに近づける */
      xnew = xg + 0.5 * (x[n] - xg);
      if (f(xnew) < f(x[n])) {
        x[n] = xnew;
      } else {
        /* 収縮: それでもよくならない場合にはx[0]以外の点をx[0]に近づける */
        for (i = 1; i <= n; ++i) {
          x[i] = x[0] + 0.5 * (x[i] - x[0]);
        }
        /* 一次元では縮小と収縮は全く同じ操作であることことに注意 */
      }
    }
  }
}
