#include <stdio.h>
#include <math.h>

double f(double x) {
  return sin(x);
}

double df(double x) {
  return cos(x);
}

int main() {
  double x, h, exact, diff;
  x = 0.3 * M_PI;
  h = 0.1;
  exact = df(x);
  diff = (f(x+h) - f(x)) / h;
  printf("%le %le %le\n", h, diff, fabs(diff - exact));
}
