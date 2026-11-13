#include "knuth_dp.h"
#include "../interval_dp/merge_piles.h"

#include <stdexcept>

int main() {
  for (int n = 0, cases = 1; n <= 7; ++n, cases *= 3) {
    for (int code = 0; code < cases; ++code) {
      std::vector<int> piles(n);
      std::vector<long long> prefix(n + 1, 0);
      int digits = code;
      for (int i = 0; i < n; ++i) {
        piles[i] = digits % 3;
        digits /= 3;
        prefix[i + 1] = prefix[i] + piles[i];
      }
      const auto cost = [&](int l, int r) { return prefix[r + 1] - prefix[l]; };
      if (KnuthDP(n, cost) != MergePilesCost(piles)) {
        throw std::runtime_error("Knuth DP differs from cubic interval DP");
      }
    }
  }
}
