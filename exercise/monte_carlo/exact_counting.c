#include "bond_table.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv) {
  double temperature = 2.0;
  int L = 4;
  int num_sites = L * L;
  int num_bonds = 2 * L * L;
  int num_states = 1 << (L * L);
  int **bond;
  double w[2];
  double weight, sum;
  int i, j, b, s, si, sj;
  bond = alloc_imatrix(num_bonds, 2);
  init_bond_table(L, bond);
  w[0] = exp(1.0 / temperature);
  w[1] = exp(-1.0 / temperature);
  sum = 0;
  for (s = 0; s < num_states; ++s) {
    weight = 1;
    for (b = 0; b < num_bonds; ++b) {
      i = mat_elem(bond, b, 0);
      j = mat_elem(bond, b, 1);
      si = (s >> i) & 1;
      sj = (s >> j) & 1;
      if (si == sj) {
        weight *= w[0];
      } else {
        weight *= w[1];
      }
    }
    sum += weight;
  }
  printf("temperature = %15.10f\n", temperature);
  printf("L = %d\n", L);
  printf("Z = %15.10f\n", sum);
  printf("free energy = %15.10f\n", - temperature * log(sum));
  printf("free energy density = %15.10f\n", - temperature * log(sum) / num_sites);
}
