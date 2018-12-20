#include "func_2d.h"
#include <stdio.h>
#include <math.h>

double optimize_1d(double *x, const double *grad) {
  double epsilon = 0.1;
  double w = (3.0 - sqrt(5)) / 2.0; /* golden ratio */
  double a, b, c, p;
  double xa[2], xb[2], xc[2], xp[2];

  int i;

  a = 0;
  for (i = 0; i < 2; ++i) xa[i] = x[i] + a * grad[i];
  c = -epsilon;
  for (i = 0; i < 2; ++i) xc[i] = x[i] + c * grad[i];
  
  if (f(xc) > f(xa)) {
    // bisection method
    while (1) {
      b = (a + c) / 2;
      for (i = 0; i < 2; ++i) xb[i] = x[i] + b * grad[i];
      if (f(xb) < f(xa)) break;
      c = b;
      for (i = 0; i < 2; ++i) xc[i] = x[i] + c * grad[i];
    }
  } else {
    // extend method
    while (1) {
      b = c;
      for (i = 0; i < 2; ++i) xb[i] = x[i] + b * grad[i];
      c -= epsilon;
      for (i = 0; i < 2; ++i) xc[i] = x[i] + c * grad[i];
      if (f(xc) > f(xb)) break;
      a = b;
      for (i = 0; i < 2; ++i) xa[i] = x[i] + a * grad[i];
    }
  }

  // start golden section
  while (fabs(c-a) > 1.0e-6 || (f(xa)-f(xb) > 1.0e-12) || (f(xc)-f(xb) > 1.0e-12)) {
    if (fabs(c-b) > fabs(b-a)) {
      p = b + w * (c-b);
      for (i = 0; i < 2; ++i) xp[i] = x[i] + p * grad[i];
      if (f(xp) < f(xb)) {
        a = b;
        for (i = 0; i < 2; ++i) xa[i] = x[i] + a * grad[i];
        b = p;
        for (i = 0; i < 2; ++i) xb[i] = x[i] + b * grad[i];
      } else {
        c = p;
        for (i = 0; i < 2; ++i) xc[i] = x[i] + c * grad[i];
      }
    } else {
      p = b + w * (a-b);
      for (i = 0; i < 2; ++i) xp[i] = x[i] + p * grad[i];
      if (f(xp) < f(xb)) {
        c = b;
        for (i = 0; i < 2; ++i) xc[i] = x[i] + c * grad[i];
        b = p;
        for (i = 0; i < 2; ++i) xb[i] = x[i] + b * grad[i];
      } else {
        a = p;
        for (i = 0; i < 2; ++i) xa[i] = x[i] + a * grad[i];
      }
    }
  }
  return b;
}

int main(int argc, char** argv) {
  double x[2];
  x[0] = -15.0;
  x[1] = 3.0;
  double r[2];
  double p[2];
  int n = 10000;

  double alpha, beta, norm2;
  int i;
  double diff;
  
  printf("%d %f %f %f\n", 0, x[0], x[1], f(x));
  df(x, r);
  p[0] = r[0];
  p[1] = r[1];
  for (i = 1; i < n; ++i) {
    alpha = optimize_1d(x, p);
    x[0] += alpha * p[0];
    x[1] += alpha * p[1];
    norm2 = r[0] * r[0] + r[1] * r[1];
    df(x, r);
    beta = (r[0] * r[0] + r[1] * r[1]) / norm2;
    p[0] = r[0] + beta * p[0];
    p[1] = r[1] + beta * p[1];
    diff = sqrt((x[0] - minx) * (x[0] - minx) + (x[1] - miny) * (x[1] - miny));
    printf("%d %f %f %20.12f %20.12f\n", i, x[0], x[1], f(x), diff);
    if (norm2 < 1.0e-10) break;
  }
}
