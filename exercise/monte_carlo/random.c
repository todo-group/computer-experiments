#include <stdio.h>
#include "mersenne_twister.h"

int main(int argc, char** argv) {
  int seed = 12345; /* 乱数の種。種が同じ場合には同じ乱数列が生成される */
  int n = 100;
  int i;

  /* set seed of Mersenne-Twister pseudo random number generator */
  init_genrand(seed);

  /* generate (0,1) random numbers */
  for (i = 0; i < n; ++i)
    printf("%d %15.10f\n", i, genrand_real3());
}
