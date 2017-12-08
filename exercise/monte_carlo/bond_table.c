#include <stdio.h>

void init_bond_table(int L, int bond[2*L*L][2]) {
  /* for square lattice */
  /* size of bond should be [2*L*L][2] */
  int x, y, xn, yn, i, j;
  int b = 0;
  for (y = 0; y < L; ++y) {
    for (x = 0; x < L; ++x) {
      i = L * y + x;
      /* right */
      xn = (x + 1) % L;
      yn = y;
      j = L * yn + xn;
      bond[b][0] = i;
      bond[b][1] = j;
      ++b;
      /* up */
      xn = x;
      yn = (y + 1) % L;
      j = L * yn + xn;
      bond[b][0] = i;
      bond[b][1] = j;
      ++b;
    }
  }
}

/* test program of init_bond_table */

int main(int argc, char** argv) {
  int L = 4;
  int num_bonds = 2 * L * L;
  int bond[num_bonds][2];
  int i, j, b, xi, yi, xj, yj;
  init_bond_table(L, bond);
  for (b = 0; b < num_bonds; ++b) {
    i = bond[b][0];
    xi = i % L;
    yi = i / L;
    j = bond[b][1];
    xj = j % L;
    yj = j / L;
    printf("%d: %d (%d,%d) - %d (%d,%d)\n", b, i, xi, yi, j, xj, yj);
  }
}
