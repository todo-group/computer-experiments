#include "matrix_util.h"

int main() {
  long m, n;
  double **mat;
  m = 10;
  n = 5;
  mat = alloc_dmatrix(m, n);
  mat[5][3] = 3.14;
  print_dmatrix(m, n, mat);
  free_dmatrix(mat);
}
