#include "mersenne_twister.h"
#include <stdio.h>
#include <math.h>

double f(double x) {
  return (5*x + x*x) + 70*sin(x);
}

int main(int argc, char** argv) {
  double x = 2.0;
  double dx = 0.01;
  double a, b, c;
  double w = (3.0 - sqrt(5)) / 2.0; /* golden ratio */

  /* 初期囲い込み開始 */
  if (f(x-dx) > f(x) && f(x+dx) > f(x)) {
    a = x - dx;
    b = x;
    c = x + dx;
  } else if (f(x-dx) < f(x)) {
    /* 左へ進む */
    c = x;
    b = x - dx;
    dx = 2 * dx;
    while (f(b-dx) < f(b)) {
      c = b;
      b = b - dx;
      dx = 2 * dx;
    }
    a = b - dx;
  } else {
    /* 右へ進む */
    a = x;
    b = x + dx;
    dx = 2 * dx;
    while (f(b+dx) < f(b)) {
      a = b;
      b = b + dx;
      dx = 2 * dx;
    }
    c = b + dx;
  }
  /* 初期囲い込み完了 */
  printf("a = %15.10f, f(a) = %15.10f\n", a, f(a));
  printf("b = %15.10f, f(b) = %15.10f\n", b, f(b));
  printf("c = %15.10f, f(c) = %15.10f\n", c, f(c));

  /* 黄金分割法 */
  while ((c-a) > 1.0e-6 || (f(a)-f(b) > 1.0e-12) || (f(c)-f(b) > 1.0e-12)) {
    /* ... */
  }
}
