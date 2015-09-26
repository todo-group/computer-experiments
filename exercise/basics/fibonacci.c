#include <stdio.h>

int main() {
  const int n_max = 60;
  int a[n_max + 1]; /* a[0]...a[n_max] */
  int i;
  a[0] = 0;
  a[1] = 1;
  for (i = 2; i <= n_max; ++i) {
    a[i] = a[i-1] + a[i-2];
  }
  for (i = 10; i <= n_max; i += 10) {
    printf("%d %d\n", i, a[i]);
  }
  return 0;
}
