#include <iostream>
#include <vector>

int main() {
  const int n_max = 60;
  std::vector<int> a(n_max + 1); /* a[0]...a[n_max] */
  a[0] = 0;
  a[1] = 1;
  for (int i = 2; i <= n_max; ++i) {
    a[i] = a[i-1] + a[i-2];
  }
  for (int i = 10; i <= n_max; i += 10) {
    std::cout << i << ' ' << a[i] << std::endl;
  }
  return 0;
}
