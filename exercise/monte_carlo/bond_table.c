#include "bond_table.h"
#include <stdio.h>

/* test program of init_bond_table */

int main(void)
{
    const int L = 4;
    const int num_bonds = 2 * L * L;
    int** bond = alloc_imatrix(num_bonds, 2);
    init_bond_table(L, bond);
    for (int b = 0; b < num_bonds; ++b)
    {
        const int i = mat_elem(bond, b, 0);
        const int xi = i % L;
        const int yi = i / L;
        const int j = mat_elem(bond, b, 1);
        const int xj = j % L;
        const int yj = j / L;
        printf("%d: %d (%d,%d) - %d (%d,%d)\n", b, i, xi, yi, j, xj, yj);
    }
    return 0;
}
