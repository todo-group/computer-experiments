#include "func_1d.h"
#include "mersenne_twister.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    const int seed = 12345;
    double x = 12.0;
    const double dx = 0.01;
    const int n = 10000000;

    /* set seed of Mersenne-Twister pseudo random number generator */
    init_genrand(seed);

    printf("%d %lf %lf\n", 0, x, f(x));
    for (int i = 1; i < n; ++i)
    {
        const double xnew = x + 2 * dx * (genrand_real3() - 0.5);
        if (f(xnew) < f(x))
        {
            x = xnew;
        }
        if (i < 1000 || i % 100 == 0)
        {
            printf("%d %lf %lf\n", i, x, f(x));
        }
    }
    return 0;
}
