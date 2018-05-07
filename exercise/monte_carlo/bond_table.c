#include "bond_table.h"
#include <stdio.h>

/* test program of init_bond_table */

int main(int argc, char** argv) {
  int L = 4;
  int num_bonds = 2 * L * L;
  int **bond;
  int i, j, b, xi, yi, xj, yj;
  bond = alloc_imatrix(num_bonds, 2);
  init_bond_table(L, bond);
  for (b = 0; b < num_bonds; ++b) {
    i = mat_elem(bond, b, 0);
    xi = i % L;
    yi = i / L;
    j = mat_elem(bond, b, 1);
    xj = j % L;
    yj = j / L;
    printf("%d: %d (%d,%d) - %d (%d,%d)\n", b, i, xi, yi, j, xj, yj);
  }
}
