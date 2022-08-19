#include <cmath>
#include <iostream>

double f(double x)
{
    return std::sin(x);
}

double df(double x)
{
    return std::cos(x);
}

int main(void)
{
    const double x = 0.3 * M_PI;
    const double h = 0.1;
    const double exact = df(x);
    const double diff = (f(x + h) - f(x)) / h;
    std::cout << h << ' ' << diff << ' ' << std::abs(diff - exact) << std::endl;
    return 0;
}
