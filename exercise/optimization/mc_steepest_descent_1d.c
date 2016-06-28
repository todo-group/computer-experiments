#include "func_1d.h"
#include "mersenne_twister.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
  int seed = 12345;
  double x = 12.0;
  double dx = 0.01;
  int n = 10000000;

  double xnew;
  int i;
  
  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  printf("%d %f %f\n", 0, x, f(x));
  for (i = 1; i < n; ++i) {
    xnew = x + 2*dx*(genrand_real3() - 0.5);
    if (f(xnew) < f(x)) x = xnew;
    if (i < 1000 || i % 100 == 0)
      printf("%d %f %f\n", i, x, f(x));
  }
}
