#include "divide_conquer_dp.h"

#include <iostream>

int main() {
  const std::vector<long long> prefix{0, 1, 3, 6, 10};
  std::cout << DivideConquerDP(4, 2, [&](int l, int r) {
    const long long sum = prefix[r] - prefix[l];
    return sum * sum;
  }) << '\n';  // 52
}
