#include "mersenne_twister.h"
#include "func_2d.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
  int seed = 12345;
  double x[2];
  x[0] = -15.0;
  x[1] = 3.0;
  double xnew[2];
  double dx = 0.1;
  int n = 10000;

  double tinit = 10;
  double tfinal = 0.01;

  double t;
  int i;
  
  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  printf("%d %f %f %f %f\n", 0, tinit, x[0], x[1], f(x));
  for (i = 1; i < n; ++i) {
    t = tinit + i * (tfinal - tinit) / n;
    xnew[0] = x[0] + 2*dx*(genrand_real3() - 0.5);
    xnew[1] = x[1] + 2*dx*(genrand_real3() - 0.5);
    if (genrand_real3() < exp(-(f(xnew)-f(x))/t)) {
      x[0] = xnew[0];
      x[1] = xnew[1];
    }
    printf("%d %f %f %f %f\n", i, t, x[0], x[1], f(x));
  }
}
