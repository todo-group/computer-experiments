#include <complex.h>
#include <math.h>
#include <stdio.h>

int main() {
  double complex x, y;
  x = 0 + 1 * I; /* 虚数単位 */
  y = cexp(x * M_PI);
  printf("i = (%lf,%lf)\n", creal(x), cimag(x));
  printf("e^{i*pi} = (%lf,%lf)\n", creal(y), cimag(y));
}
