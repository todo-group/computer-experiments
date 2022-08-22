#include "func_2d.h"
#include <math.h>
#include <stdio.h>

double optimize_1d(double* x, const double* grad)
{
    const double epsilon = 0.1;
    const double w = (3.0 - sqrt(5)) / 2.0; /* golden ratio */
    double xa[2], xb[2], xc[2], xp[2];

    double a = 0;
    for (int i = 0; i < 2; ++i)
    {
        xa[i] = x[i] + a * grad[i];
    }
    double c = -epsilon;
    for (int i = 0; i < 2; ++i)
    {
        xc[i] = x[i] + c * grad[i];
    }

    double b;
    if (f(xc) > f(xa))
    {
        // bisection method
        while (1)
        {
            b = (a + c) / 2;
            for (int i = 0; i < 2; ++i)
            {
                xb[i] = x[i] + b * grad[i];
            }
            if (f(xb) < f(xa))
            {
                break;
            }
            c = b;
            for (int i = 0; i < 2; ++i)
            {
                xc[i] = x[i] + c * grad[i];
            }
        }
    }
    else
    {
        // extend method
        while (1)
        {
            b = c;
            for (int i = 0; i < 2; ++i)
            {
                xb[i] = x[i] + b * grad[i];
            }
            c -= epsilon;
            for (int i = 0; i < 2; ++i)
            {
                xc[i] = x[i] + c * grad[i];
            }
            if (f(xc) > f(xb))
            {
                break;
            }
            a = b;
            for (int i = 0; i < 2; ++i)
            {
                xa[i] = x[i] + a * grad[i];
            }
        }
    }

    // start golden section
    while (fabs(c - a) > 1.0e-6 || (f(xa) - f(xb) > 1.0e-12) || (f(xc) - f(xb) > 1.0e-12))
    {
        if (fabs(c - b) > fabs(b - a))
        {
            const double p = b + w * (c - b);
            for (int i = 0; i < 2; ++i)
            {
                xp[i] = x[i] + p * grad[i];
            }
            if (f(xp) < f(xb))
            {
                a = b;
                for (int i = 0; i < 2; ++i)
                {
                    xa[i] = x[i] + a * grad[i];
                }
                b = p;
                for (int i = 0; i < 2; ++i)
                {
                    xb[i] = x[i] + b * grad[i];
                }
            }
            else
            {
                c = p;
                for (int i = 0; i < 2; ++i)
                {
                    xc[i] = x[i] + c * grad[i];
                }
            }
        }
        else
        {
            const double p = b + w * (a - b);
            for (int i = 0; i < 2; ++i)
            {
                xp[i] = x[i] + p * grad[i];
            }
            if (f(xp) < f(xb))
            {
                c = b;
                for (int i = 0; i < 2; ++i)
                {
                    xc[i] = x[i] + c * grad[i];
                }
                b = p;
                for (int i = 0; i < 2; ++i)
                {
                    xb[i] = x[i] + b * grad[i];
                }
            }
            else
            {
                a = p;
                for (int i = 0; i < 2; ++i)
                {
                    xa[i] = x[i] + a * grad[i];
                }
            }
        }
    }
    return b;
}

int main(void)
{
    double x[2] = { -15.0, 3.0 };
    const int n = 10000;

    printf("%d %lf %lf %lf\n", 0, x[0], x[1], f(x));
    double r[2];
    df(x, r);
    double p[2] = { r[0], r[1] };
    for (int i = 1; i < n; ++i)
    {
        const double alpha = optimize_1d(x, p);
        x[0] += alpha * p[0];
        x[1] += alpha * p[1];
        const double norm2 = r[0] * r[0] + r[1] * r[1];
        df(x, r);
        const double beta = (r[0] * r[0] + r[1] * r[1]) / norm2;
        p[0] = r[0] + beta * p[0];
        p[1] = r[1] + beta * p[1];
        const double diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
        printf("%d %lf %lf %20.12lf %20.12lf\n", i, x[0], x[1], f(x), diff);
        if (norm2 < 1.0e-10)
        {
            break;
        }
    }
    return 0;
}
