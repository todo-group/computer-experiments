#include "cmatrix.h"
#include <stdio.h>

void init_square_lattice(int L, int **neighbor) {
  /* size of neighbors should be (L*L) x 4 */
  int x, y, xn, yn, i, j;
  for (y = 0; y < L; ++y) {
    for (x = 0; x < L; ++x) {
      i = L * y + x;
      /* right */
      xn = (x + 1) % L;
      yn = y;
      j = L * yn + xn;
      mat_elem(neighbor, i, 0) = j;
      /* left */
      xn = (L + x - 1) % L;
      yn = y;
      j = L * yn + xn;
      mat_elem(neighbor, i, 1) = j;
      /* up */
      xn = x;
      yn = (y + 1) % L;
      j = L * yn + xn;
      mat_elem(neighbor, i, 2) = j;
      /* down */
      xn = x;
      yn = (L + y - 1) % L;
      j = L * yn + xn;
      mat_elem(neighbor, i, 3) = j;
    }
  }
}

/* test program of init_square_lattice */

int main(int argc, char** argv) {
  int L = 4;
  int **neighbor;
  int i, x, y;
  neighbor = alloc_imatrix(L*L, 4);
  init_square_lattice(L, neighbor);
  for (i = 0; i < L * L; ++i) {
    x = i % L;
    y = i / L;
    printf("%d (%d,%d): %d %d %d %d\n", i, x, y,
           mat_elem(neighbor, i, 0), mat_elem(neighbor, i, 1),
           mat_elem(neighbor, i, 2), mat_elem(neighbor, i, 3));
  }
}
