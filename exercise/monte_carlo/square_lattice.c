#include "matrix_util.h"
#include <stdio.h>

void init_square_lattice(int L, int neighbor[L*L][4]) {
  /* size of neighbors should be [L*L][4] */
  int x, y, xn, yn, i, j;
  for (y = 0; y < L; ++y) {
    for (x = 0; x < L; ++x) {
      i = L * y + x;
      /* right */
      xn = (x + 1) % L;
      yn = y;
      j = L * yn + xn;
      neighbor[i][0] = j;
      /* left */
      xn = (L + x - 1) % L;
      yn = y;
      j = L * yn + xn;
      neighbor[i][1] = j;
      /* up */
      xn = x;
      yn = (y + 1) % L;
      j = L * yn + xn;
      neighbor[i][2] = j;
      /* down */
      xn = x;
      yn = (L + y - 1) % L;
      j = L * yn + xn;
      neighbor[i][3] = j;
    }
  }
}

/* test program of init_square_lattice */

int main(int argc, char** argv) {
  int L = 4;
  int neighbor[L*L][4];
  int i, x, y;
  init_square_lattice(L, neighbor);
  for (i = 0; i < L * L; ++i) {
    x = i % L;
    y = i / L;
    printf("%d (%d,%d): %d %d %d %d\n", i, x, y,
           neighbor[i][0], neighbor[i][1],
           neighbor[i][2], neighbor[i][3]);
  }
}
