#include <math.h>
#include <stdio.h>

double f(double x)
{
    return sin(x);
}

double df(double x)
{
    return cos(x);
}

int main(void)
{
    const double x = 0.3 * M_PI;
    const double h = 0.1;
    const double exact = df(x);
    const double diff = (f(x + h) - f(x)) / h;
    printf("%le %le %le\n", h, diff, fabs(diff - exact));
    return 0;
}
