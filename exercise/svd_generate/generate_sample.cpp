#include <boost/random.hpp>
#include <cmath>
#include <iostream>
#include <vector>

double f(double a0, double a1, double a2, double c0, double c1, double c2, double x)
{
    return a0 + a1 * x + a2 * x * x + c0 * exp(-(x - c1) * (x - c1) / (2 * c2));
}

int main(void)
{
    double x0, x1;
    int n;
    double sigma;
    double a0, a1, a2, c0, c1, c2;
    std::cin >> x0 >> x1 >> n >> sigma >> a0 >> a1 >> a2 >> c0 >> c1 >> c2;

    boost::mt19937 engine(12345l);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<>>
        gauss(engine, boost::normal_distribution<>(0, sigma));
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<>>
        gauss2(engine, boost::normal_distribution<>(sigma, 0.2 * sigma));

    const double dx = (x1 - x0) / (n - 1);
    std::vector<double> x, y, ye;
    for (int i = 0; i < n; ++i)
    {
        const double t = x0 + dx * i;
        x.push_back(t);
        y.push_back(f(a0, a1, a2, c0, c1, c2, t) + gauss());
        ye.push_back(gauss2());
    }
    for (int i = 0; i < n; ++i)
    {
        std::cout << x[i] << ' ' << y[i] << ' ' << ye[i] << std::endl;
    }
    return 0;
}
