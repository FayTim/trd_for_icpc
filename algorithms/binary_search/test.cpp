#include "binary_search.h"

#include <algorithm>
#include <climits>
#include <iostream>
#include <numeric>
#include <optional>
#include <vector>

bool Check(bool ok, const char* message) {
  if (!ok) std::cerr << message << '\n';
  return ok;
}

int main() {
  using answer_search::FirstTrue;
  using answer_search::LastTrue;
  for (long long lo = -5; lo <= 5; ++lo) {
    for (long long hi = -5; hi <= 5; ++hi) {
      for (long long threshold = -7; threshold <= 7; ++threshold) {
        std::optional<long long> first, last;
        for (long long x = lo; x <= hi; ++x) {
          if (x >= threshold && !first) first = x;
          if (x <= threshold) last = x;
        }
        if (!Check(FirstTrue(lo, hi, [&](long long x) {
                     return x >= threshold;
                   }) == first, "FirstTrue differs from exhaustive scan") ||
            !Check(LastTrue(lo, hi, [&](long long x) {
                     return x <= threshold;
                   }) == last, "LastTrue differs from exhaustive scan")) {
          return 1;
        }
      }
    }
  }

  int empty_calls = 0;
  const auto count_empty = [&](long long) { ++empty_calls; return true; };
  if (!Check(!FirstTrue(1, 0, count_empty) &&
             !LastTrue(1, 0, count_empty) && empty_calls == 0,
             "Empty interval must not evaluate the predicate")) return 1;

  for (long long threshold : {LLONG_MIN, LLONG_MIN + 1, -1LL, 0LL, 1LL,
                              LLONG_MAX - 1, LLONG_MAX}) {
    int calls = 0;
    auto first = FirstTrue(LLONG_MIN, LLONG_MAX, [&](long long x) {
      ++calls;
      return x >= threshold;
    });
    if (!Check(first == threshold && calls <= 65,
               "FirstTrue failed across full signed range")) return 1;
    calls = 0;
    auto last = LastTrue(LLONG_MIN, LLONG_MAX, [&](long long x) {
      ++calls;
      return x <= threshold;
    });
    if (!Check(last == threshold && calls <= 65,
               "LastTrue failed across full signed range")) return 1;
  }
  for (long long x : {LLONG_MIN, 0LL, LLONG_MAX}) {
    if (!Check(FirstTrue(x, x, [](long long) { return true; }) == x &&
               LastTrue(x, x, [](long long) { return true; }) == x &&
               !FirstTrue(x, x, [](long long) { return false; }) &&
               !LastTrue(x, x, [](long long) { return false; }),
               "Singleton interval failed")) return 1;
  }
  if (!Check(!FirstTrue(LLONG_MIN, LLONG_MAX,
                       [](long long) { return false; }) &&
             !LastTrue(LLONG_MIN, LLONG_MAX,
                       [](long long) { return false; }),
             "All-false predicate must have no answer")) return 1;

  // Compare answer search + greedy check with every partition into segments.
  for (int n = 1; n <= 5; ++n) {
    int configurations = 1;
    for (int i = 0; i < n; ++i) configurations *= 4;
    for (int code = 0; code < configurations; ++code) {
      std::vector<long long> weights(n);
      int value = code;
      for (auto& weight : weights) { weight = value % 4 + 1; value /= 4; }
      std::vector<long long> expected(n + 1, LLONG_MAX);
      for (int cuts = 0; cuts < (1 << (n - 1)); ++cuts) {
        int groups = 1;
        long long sum = 0, largest = 0;
        for (int i = 0; i < n; ++i) {
          sum += weights[i];
          if (i == n - 1 || (cuts & (1 << i))) {
            largest = std::max(largest, sum);
            sum = 0;
            if (i != n - 1) ++groups;
          }
        }
        for (int k = groups; k <= n; ++k)
          expected[k] = std::min(expected[k], largest);
      }
      for (int k = 1; k <= n; ++k) {
        const auto feasible = [&](long long capacity) {
          int groups = 1;
          long long sum = 0;
          for (long long weight : weights) {
            if (sum > capacity - weight) { ++groups; sum = 0; }
            sum += weight;
          }
          return groups <= k;
        };
        const auto actual = FirstTrue(
            *std::max_element(weights.begin(), weights.end()),
            std::accumulate(weights.begin(), weights.end(), 0LL), feasible);
        if (!Check(actual == expected[k],
                   "Capacity search differs from exhaustive partitions")) return 1;
      }
    }
  }
}
