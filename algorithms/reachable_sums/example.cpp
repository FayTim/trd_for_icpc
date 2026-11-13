#include "reachable_sums.h"
#include <iostream>

int main() {
  const auto sums = ReachableSums<6>({2, 3, 3});
  std::cout << sums.test(6) << ' ' << sums[4] << '\n';  // 1 0
}
