#include "cmatrix.h"

void init_bond_table(int L, int **bond) {
  /* for square lattice */
  /* size of bond should be (2*L*L) x 2 */
  int x, y, xn, yn, i, j;
  int b = 0;
  for (y = 0; y < L; ++y) {
    for (x = 0; x < L; ++x) {
      i = L * y + x;
      /* right */
      xn = (x + 1) % L;
      yn = y;
      j = L * yn + xn;
      mat_elem(bond, b, 0) = i;
      mat_elem(bond, b, 1) = j;
      ++b;
      /* up */
      xn = x;
      yn = (y + 1) % L;
      j = L * yn + xn;
      mat_elem(bond, b, 0) = i;
      mat_elem(bond, b, 1) = j;
      ++b;
    }
  }
}
