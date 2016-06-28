#include "func_2d.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
  double x[2];
  x[0] = -15.0;
  x[1] = 3.0;
  double grad[2];
  double epsilon = 0.0018;
  int n = 10000;

  int i;
  
  printf("%d %f %f %f\n", 0, x[0], x[1], f(x));
  for (i = 1; i < n; ++i) {
    df(x, grad);
    x[0] -= epsilon * grad[0];
    x[1] -= epsilon * grad[1];
    printf("%d %f %f %f\n", i, x[0], x[1], f(x));
  }
}
