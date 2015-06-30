#include "mersenne_twister.h"
#include <stdio.h>
#include <math.h>

double f(double x) {
  return (5*x + x*x) + 70*sin(x);
}

int main(int argc, char** argv) {
  int seed = 12345;
  double x = 12.0;
  double dx = 0.01;
  int n = 1000;

  double xnew;
  int i;
  
  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  for (i = 0; i < n; ++i) {
    xnew = x + 2*dx*(genrand_real3() - 0.5);
    if (f(xnew) < f(x)) x = xnew;
    printf("%d %f %f\n", i, x, f(x));
  }
  
}
