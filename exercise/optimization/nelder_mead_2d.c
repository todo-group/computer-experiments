#include "func_2d.h"
#include <math.h>
#include <stdio.h>

/* v0 と v1 の値を交換する関数 */
void swap(double* v0, double* v1)
{
    const double vt = *v0;
    *v0 = *v1;
    *v1 = vt;
}

int main(void)
{
    const int n = 2; /* 次元 (simplexの頂点数はn+1) */
    double x[n + 1][n];

    /* 初期値の設定 (3点が一直線上にのらないように選ぶ) */
    x[0][0] = -15.0;
    x[0][1] = 3.0;
    x[1][0] = x[0][0] + 0.1;
    x[1][1] = x[0][1];
    x[2][0] = x[0][0];
    x[2][1] = x[0][1] + 0.1;

    while (1)
    {
        double y[n + 1];
        /* 関数値の評価 */
        for (int i = 0; i <= n; ++i)
        {
            y[i] = f(x[i]);
        }

        /* y[n] が最大値、y[n-1]が二番目大きな値、y[0] が最小値となるように並び替える */
        for (int i = 0; i < n; ++i)
        {
            if (y[i] > y[n])
            {
                for (int j = 0; j < n; ++j)
                {
                    swap(&x[i][j], &x[n][j]);
                }
                swap(&y[i], &y[n]);
            }
        }
        for (int i = 1; i < n; ++i)
        {
            if (y[i] < y[0])
            {
                for (int j = 0; j < n; ++j)
                {
                    swap(&x[i][j], &x[0][j]);
                }
                swap(&y[i], &y[0]);
            }
        }
        for (int i = 1; i < n - 1; ++i)
        {
            if (y[i] > y[n - 1])
            {
                for (int j = 0; j < n; ++j)
                {

                    swap(&x[i][j], &x[n - 1][j]);
                }
                swap(&y[i], &y[n - 1]);
            }
        }

        double diff;
        for (int i = 0; i <= n; ++i)
        {
            diff = sqrt((x[i][0] - minx) * (x[i][0] - minx) + (x[i][1] - miny) * (x[i][1] - miny));
            printf("(x[%d],y[%d]) = (%15.10lf,%15.10lf ) %20.12lf %20.12lf\n",
                i, i, x[i][0], x[i][1], y[i], diff);
        }

        /* 収束判定 */
        diff = 0.0;
        for (int j = 0; j < n; ++j)
        {
            diff += (x[n][j] - x[0][j]) * (x[n][j] - x[0][j]);
        }
        diff = sqrt(diff);
        if (diff < 1.0e-6 && fabs(y[n] - y[0]) < 1.0e-12)
        {
            break;
        }

        /* 最大値をのぞいた点の重心 */
        double xg[n];
        for (int i = 0; i < n; ++i)
        {
            xg[i] = 0.0;
        }
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                xg[j] += x[i][j];
            }
        }
        for (int j = 0; j < n; ++j)
        {
            xg[j] /= n;
        }

        double xnew[n];
        /* 反射: x[n]をxgに関する対称な点に移動 */
        for (int j = 0; j < n; ++j)
        {
            xnew[j] = xg[j] - (x[n][j] - xg[j]);
        }
        if (f(xnew) < y[n])
        {
            for (int j = 0; j < n; ++j)
            {
                x[n][j] = xnew[j];
            }
        }
        if (f(xnew) < y[0])
        {
            /* 拡大: 反射点がy[0]よりも小さくなるようであればさらに先に進む */
            for (int j = 0; j < n; ++j)
            {
                xnew[j] = xg[j] + 2 * (x[n][j] - xg[j]);
            }
            if (f(xnew) < y[0])
            {
                for (int j = 0; j < n; ++j)
                {
                    x[n][j] = xnew[j];
                }
            }
        }
        else if (f(x[n]) > y[n - 1])
        {
            /* 縮小: 二番目に悪い値よりもまだ大きい場合にはxgに近づける */
            for (int j = 0; j < n; ++j)
            {
                xnew[j] = xg[j] + 0.5 * (x[n][j] - xg[j]);
            }
            if (f(xnew) < f(x[n]))
            {
                for (int j = 0; j < n; ++j)
                {
                    x[n][j] = xnew[j];
                }
            }
            else
            {
                /* 収縮: それでもよくならない場合にはx[0]以外の点をx[0]に近づける */
                for (int i = 1; i <= n; ++i)
                {
                    for (int j = 0; j < n; ++j)
                    {
                        x[i][j] = x[0][j] + 0.5 * (x[i][j] - x[0][j]);
                    }
                }
            }
        }
    }
    return 0;
}
