#include "func_2d.h"
#include "mersenne_twister.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
  int seed = 12345;
  double x[2];
  x[0] = -15.0;
  x[1] = 3.0;
  double xnew[2];
  double dx = 0.1;
  int n = 1000;

  int i;
  double diff;
  
  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  printf("%d %f %f %f\n", 0, x[0], x[1], f(x));
  for (i = 1; i < n; ++i) {
    xnew[0] = x[0] + 2*dx*(genrand_real3() - 0.5);
    xnew[1] = x[1] + 2*dx*(genrand_real3() - 0.5);
    if (f(xnew) < f(x)) {
      x[0] = xnew[0];
      x[1] = xnew[1];
    }
    diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
    printf("%d %f %f %20.12f %20.12f\n", i, x[0], x[1], f(x), diff);
  }
}
