#include "binary_search.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
  const std::vector<long long> weights{7, 2, 5, 10, 8};
  const int max_groups = 2;
  const long long lo = *std::max_element(weights.begin(), weights.end());
  const long long hi = std::accumulate(weights.begin(), weights.end(), 0LL);

  const auto feasible = [&](long long capacity) {
    int groups = 1;
    long long current = 0;
    for (long long weight : weights) {
      if (current > capacity - weight) {
        if (++groups > max_groups) return false;
        current = 0;
      }
      current += weight;
    }
    return true;
  };
  // At hi all items fit in one group, so the answer exists.
  std::cout << *answer_search::FirstTrue(lo, hi, feasible) << '\n';  // 18
}
