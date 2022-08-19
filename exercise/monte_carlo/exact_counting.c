#include "bond_table.h"
#include <math.h>
#include <stdio.h>

int main(void)
{
    const double temperature = 2.0;
    const int L = 4;
    const int num_sites = L * L;
    const int num_bonds = 2 * L * L;
    const int num_states = 1 << (L * L);
    const double w[2] = { exp(1.0 / temperature), exp(-1.0 / temperature) };
    int** bond = alloc_imatrix(num_bonds, 2);
    init_bond_table(L, bond);
    double sum = 0;
    for (int s = 0; s < num_states; ++s)
    {
        double weight = 1;
        for (int b = 0; b < num_bonds; ++b)
        {
            const int i = mat_elem(bond, b, 0);
            const int j = mat_elem(bond, b, 1);
            const int si = (s >> i) & 1;
            const int sj = (s >> j) & 1;
            if (si == sj)
            {
                weight *= w[0];
            }
            else
            {
                weight *= w[1];
            }
        }
        sum += weight;
    }
    printf("temperature = %15.10f\n", temperature);
    printf("L = %d\n", L);
    printf("Z = %15.10f\n", sum);
    printf("free energy = %15.10f\n", -temperature * log(sum));
    printf("free energy density = %15.10f\n", -temperature * log(sum) / num_sites);
    return 0;
}
