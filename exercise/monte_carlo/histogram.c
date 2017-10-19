#include "mersenne_twister.h"
#include "matrix_util.h"
#include <stdio.h>
#include <math.h>

/* uniform random number on (0,1) */
double ran() {
  double x = genrand_real3();
  return x;
}

int main(int argc, char** argv) {
  int seed = 12345;
  int samples = 1024;
  int bins = 16;
  double xmin = 0.0;
  double xmax = 1.0;
  double dx, dh;
  double *hist;
  int i, j;
  double x, ave, err;

  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  /* prepare vector for histogram */
  dx = (xmax - xmin) / bins;
  dh = 1.0 / (samples * dx);
  hist = alloc_dvector(bins);
  for (j = 0; j < bins; ++j) hist[j] = 0.0;
  
  /* generate histogram */
  for (i = 0; i < samples; ++i) {
    x = ran();
    j = (x - xmin) / dx;
    hist[j] += 1.0;
  }

  for (j = 0; j < bins; ++j) {
    x = (j + 0.5) * dx;
    ave = hist[j] * dh;
    err = sqrt(hist[j]) * dh;
    printf("%d %15.10f %15.10f %15.10f\n", j, x, ave, err);
  }

  free_dvector(hist);
}
