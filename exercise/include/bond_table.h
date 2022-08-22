#ifndef BOND_TABLE_H
#define BOND_TABLE_H

#include "cmatrix.h"

void init_bond_table(int L, int** const bond)
{
    /* for square lattice */
    /* size of bond should be (2*L*L) x 2 */
    int b = 0;
    for (int y = 0; y < L; ++y)
    {
        for (int x = 0; x < L; ++x)
        {
            int i, j, xn, yn;
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

#endif // BOND_TABLE_H