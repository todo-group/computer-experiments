#include "func_2d.h"
#include "mersenne_twister.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    const int seed = 12345;
    double x[2] = { -15.0, 3.0 };
    const double dx = 0.1;
    const int n = 10000;

    const double tinit = 10;
    const double tfinal = 0.01;

    /* set seed of Mersenne-Twister pseudo random number generator */
    init_genrand(seed);

    double diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
    printf("%d %lf %lf %lf %20.12lf %20.12lf\n", 0, 0.0, x[0], x[1], f(x), diff);
    for (int i = 1; i < n; ++i)
    {
        const double t = tinit + i * (tfinal - tinit) / n;
        const double xnew[2] = {
            x[0] + 2 * dx * (genrand_real3() - 0.5),
            x[1] + 2 * dx * (genrand_real3() - 0.5)
        };
        if (genrand_real3() < exp(-(f(xnew) - f(x)) / t))
        {
            x[0] = xnew[0];
            x[1] = xnew[1];
        }
        diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
        printf("%d %lf %lf %lf %20.12lf %20.12lf\n", i, t, x[0], x[1], f(x), diff);
    }
    return 0;
}
