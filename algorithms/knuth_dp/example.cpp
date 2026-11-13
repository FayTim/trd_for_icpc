#include "knuth_dp.h"

#include <iostream>

int main() {
  const std::vector<long long> prefix{0, 4, 7, 10, 14};
  std::cout << KnuthDP(4, [&](int l, int r) {
    return prefix[r + 1] - prefix[l];
  }) << '\n';  // 28
}
