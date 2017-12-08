#include <stdio.h>
#include <math.h>

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

int main(int argc, char** argv) {
  double temperature = 2.0;
  int L = 4;
  int num_sites = L * L;
  int num_bonds = 2 * L * L;
  int num_states = 1 << (L * L);
  int bond[num_bonds][2];
  double w[2];
  double weight, sum;
  int i, j, b, s, si, sj;
  init_bond_table(L, bond);
  w[0] = exp(1.0 / temperature);
  w[1] = exp(-1.0 / temperature);
  sum = 0;
  for (s = 0; s < num_states; ++s) {
    weight = 1;
    for (b = 0; b < num_bonds; ++b) {
      i = bond[b][0];
      j = bond[b][1];
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
