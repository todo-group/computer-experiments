#include <stdio.h>
#include <math.h>

double f(double x) { return sin(x); }

int main() {
  double a, b, x;
  double delta = 1.0e-1;
  double epsilon = 1.0e-8;
  a = 0.3 * M_PI;
  b = a + delta;
  printf("Find initial enclosure\n");
  while (f(a) * f(b) > 0) {
    printf("f(%le) = %le, f(%le) = %le\n", a, f(a), b, f(b));
    b += delta;
  }
  printf("Start bisection search\n");
  while (fabs(b - a) > epsilon) {
    printf("f(%le) = %le, f(%le) = %le\n", a, f(a), b, f(b));
    x = (a + b) / 2;
    if (fabs(f(x)) < epsilon) break;
    if (f(a) * f(x) < 0) {
      b = x;
    } else {
      a = x;
    }
  }
  printf("Solution = %le\n", x);
}
