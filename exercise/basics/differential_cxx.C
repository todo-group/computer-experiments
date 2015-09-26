#include <iostream>
#include <cmath>

double f(double x) {
  return std::sin(x);
}

double df(double x) {
  return std::cos(x);
}

int main() {
  double x, h, exact, diff;
  x = 0.3 * M_PI;
  h = 0.1;
  exact = df(x);
  diff = (f(x+h) - f(x)) / h;
  std::cout << h << ' ' << diff << ' ' << std::abs(diff - exact) << std::endl;
}
