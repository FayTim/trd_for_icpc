#include "divide_conquer_dp.h"

#include <limits>
#include <stdexcept>

template <class Cost>
long long Exhaustive(int l, int n, int groups, Cost cost) {
  if (groups == 1) return cost(l, n);
  long long best = std::numeric_limits<long long>::max();
  for (int r = l + 1; r <= n - groups + 1; ++r) {
    best = std::min(best, cost(l, r) + Exhaustive(r, n, groups - 1, cost));
  }
  return best;
}

int main() {
  for (int n = 1; n <= 8; ++n) {
    for (int mask = 0; mask < (1 << n); ++mask) {
      std::vector<long long> prefix(n + 1, 0);
      for (int i = 0; i < n; ++i) {
        prefix[i + 1] = prefix[i] + ((mask >> i) & 1) * (i + 1);
      }
      const auto cost = [&](int l, int r) {
        const long long sum = prefix[r] - prefix[l];
        return sum * sum;
      };
      for (int groups = 1; groups <= n; ++groups) {
        if (DivideConquerDP(n, groups, cost) != Exhaustive(0, n, groups, cost)) {
          throw std::runtime_error("Divide-and-conquer DP differs from exhaustive partitioning");
        }
      }
    }
  }
}
