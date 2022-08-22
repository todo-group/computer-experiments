#include "func_1d.h"
#include "mersenne_twister.h"
#include <stdio.h>

int main(void)
{
    const int seed = 12345;
    double x = 12.0;
    const double dx = 0.1;
    const int n = 10000000;

    const double tinit = 100;
    const double tfinal = 1;

    /* set seed of Mersenne-Twister pseudo random number generator */
    init_genrand(seed);

    for (int i = 0; i < n; ++i)
    {
        const double t = tinit + i * (tfinal - tinit) / n;
        const double xnew = x + 2 * dx * (genrand_real3() - 0.5);
        if (genrand_real3() < exp(-(f(xnew) - f(x)) / t))
        {
            x = xnew;
        }
        if (i < 1000 || i % 100 == 0)
        {
            printf("%d %lf %lf %lf\n", i, t, x, f(x));
        }
    }
    return 0;
}
