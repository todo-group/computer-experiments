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
  double diff;
  
  printf("%d %f %f %f\n", 0, x[0], x[1], f(x));
  for (i = 1; i < n; ++i) {
    df(x, grad);
    x[0] -= epsilon * grad[0];
    x[1] -= epsilon * grad[1];
    diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
    printf("%d %f %f %20.12f %20.12f\n", i, x[0], x[1], f(x), diff);
    if (grad[0] * grad[0] + grad[1] * grad[1] < 1.0e-10) break;
  }
}
