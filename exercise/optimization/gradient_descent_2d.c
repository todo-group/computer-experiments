#include "func_2d.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    double x[2] = { -15.0, 3.0 };
    const double epsilon = 0.0018;
    const int n = 10000;

    printf("%d %lf %lf %lf\n", 0, x[0], x[1], f(x));
    for (int i = 1; i < n; ++i)
    {
        double grad[2];
        df(x, grad);
        x[0] -= epsilon * grad[0];
        x[1] -= epsilon * grad[1];
        const double diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
        printf("%d %lf %lf %20.12lf %20.12lf\n", i, x[0], x[1], f(x), diff);
        if (grad[0] * grad[0] + grad[1] * grad[1] < 1.0e-10)
        {
            break;
        }
    }
    return 0;
}
