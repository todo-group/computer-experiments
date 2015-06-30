#include "mersenne_twister.h"
#include <stdio.h>
#include <math.h>

double f(double x) {
  return (5*x + x*x) + 70*sin(x);
}

int main(int argc, char** argv) {
  int seed = 12345;
  double x = 12.0;
  double dx = 0.1;
  int n = 10000000;

  double tinit = 100;
  double tfinal = 1;

  double t, xnew;
  int i;
  
  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  for (i = 0; i < n; ++i) {
    t = tinit + i * (tfinal - tinit) / n;
    xnew = x + 2*dx*(genrand_real3() - 0.5);
    if (genrand_real3() < exp(-(f(xnew)-f(x))/t)) x = xnew;
    if (i % 100 == 0)
      printf("%d %f %f %f\n", i, t, x, f(x));
  }
}
