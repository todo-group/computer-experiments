#include "mersenne_twister.h"
#include <math.h>
#include <stdio.h>

double v(double x) {
  return x * x;
}
  
int main(int argc, char** argv) {
  int seed = 12345;
  double beta = 1.0;
  double delta = 10.0;
  int n = 1000;
  int i;
  double x, trial;
  double x2 = 0.0;

  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  /* Monte Carlo steps */
  x = 0.0;
  for (i = 0; i < n; ++i) {
    trial = x + delta * (2 * genrand_real3() - 1);
    if (genrand_real3() < exp(-beta * (v(trial)-v(x)))) x = trial;
    x2 += x * x;
    printf("%d %10.5f\n", i, x);
  }

  x2 /= n;
  printf("# average of x^2 = %10.5f\n", x2);
}
