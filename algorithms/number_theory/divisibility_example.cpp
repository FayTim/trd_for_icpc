#include "divisibility.h"
#include <iostream>

int main() {
  std::vector<std::int64_t> frequency(7);
  for (int x : {2, 3, 4, 6}) ++frequency[x];
  auto exact = nt::ExactGcdPairs(frequency);
  std::cout << exact[1] << ' ' << exact[2] << '\n';  // 2 3
  std::cout << nt::FactorialValuation(10, 2) << '\n';  // 8
  std::uint64_t sum = 0;
  nt::QuotientBlocks(10, [&](auto l, auto r, auto q) {
    sum += (r - l + 1) * q;
  });
  std::cout << sum << '\n';  // 27: sum of divisor counts for 1..10
}
