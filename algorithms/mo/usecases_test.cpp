#include "approximate_lis.h"
#include "range_statistics.h"

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <vector>

namespace {

void Require(bool ok, const char* message) {
  if (!ok) {
    std::cerr << message << '\n';
    std::exit(1);
  }
}

int BruteLis(const std::vector<int>& a, int l, int r) {
  std::vector<int> dp(r - l, 1);
  int result = 0;
  for (int i = l; i < r; ++i) {
    for (int j = l; j < i; ++j)
      if (a[j] < a[i]) dp[i - l] = std::max(dp[i - l], dp[j - l] + 1);
    result = std::max(result, dp[i - l]);
  }
  return result;
}

std::vector<mo::Query> AllIntervals(int n) {
  std::vector<mo::Query> queries;
  for (int l = 0; l <= n; ++l)
    for (int r = l; r <= n; ++r) queries.push_back({l, r});
  return queries;
}

void CheckLis(const std::vector<int>& a) {
  auto queries = AllIntervals(a.size());
  auto result = mo::ApproximateRangeLis(a, queries);
  for (int id = 0; id < int(queries.size()); ++id) {
    auto q = queries[id];
    int expected = BruteLis(a, q.l, q.r), got = result[id];
    if (2 * expected > 3 * got || 2 * got > 3 * expected) {
      std::cerr << "LIS bound violated at [" << q.l << ',' << q.r
                << "): exact=" << expected << ", got=" << got << '\n';
      std::exit(1);
    }
  }
}

void CheckStatistics(const std::vector<int>& a, std::mt19937& rng) {
  auto queries = AllIntervals(a.size());
  // Repeated and shuffled queries must keep input order.
  if (!queries.empty()) queries.push_back(queries.front());
  std::shuffle(queries.begin(), queries.end(), rng);
  std::vector<mo::FrequencySummary> expected_stats;
  std::vector<mo::KthQuery> kth_queries;
  std::vector<int> expected_kth;
  for (auto q : queries) {
    std::map<int, int> count;
    for (int i = q.l; i < q.r; ++i) ++count[a[i]];
    mo::FrequencySummary summary;
    summary.distinct = count.size();
    for (auto [value, c] : count) {
      (void)value;
      summary.equal_pairs += 1LL * c * (c - 1) / 2;
      summary.squared_frequencies += 1LL * c * c;
    }
    expected_stats.push_back(summary);
    std::vector<int> sorted(a.begin() + q.l, a.begin() + q.r);
    std::sort(sorted.begin(), sorted.end());
    for (int k = 1; k <= q.r - q.l; ++k) {
      kth_queries.push_back({q.l, q.r, k});
      expected_kth.push_back(sorted[k - 1]);
    }
  }
  Require(mo::RangeFrequencies(a, queries) == expected_stats,
          "Frequency statistics differ from brute force");
  Require(mo::RangeKth(a, kth_queries) == expected_kth,
          "Bucket kth statistic differs from sorting");
  CheckLis(a);
}

void CheckLargeCounts() {
  constexpr int n = 100000;
  std::vector<mo::Query> queries{{0, n}, {0, 0}, {n, n}, {n - 1, n}};
  auto stats = mo::RangeFrequencies(std::vector<int>(n, INT_MIN), queries);
  Require(stats[0] == mo::FrequencySummary{1, 1LL * n * (n - 1) / 2,
                                         1LL * n * n},
          "Frequency arithmetic must be 64-bit");
  std::vector<int> decreasing(n);
  std::iota(decreasing.rbegin(), decreasing.rend(), 0);
  Require(mo::ApproximateRangeLis(decreasing, queries) ==
              std::vector<int>{1, 0, 0, 1},
          "Decreasing intervals have LIS one");
}

}  // namespace

int main() {
  std::mt19937 rng(0x4d4f2025);
  for (const auto& a : std::vector<std::vector<int>>{
           {}, {7}, {5, 5, 5, 5}, {1, 2, 3, 4}, {4, 3, 2, 1},
           {4, 1, 2, 1, 3}, {INT_MIN, INT_MAX, 0, INT_MIN, -1},
           {INT_MAX, 0, INT_MIN}, {2, 1, 4, 3}})
    CheckStatistics(a, rng);
  for (int trial = 0; trial < 300; ++trial) {
    std::vector<int> a(rng() % 18);
    for (int& value : a) value = int(rng() % 11) - 5;
    CheckStatistics(a, rng);
  }
  // All permutations through n=7, every interval; independent O(n^2) LIS.
  for (int n = 1; n <= 7; ++n) {
    std::vector<int> a(n);
    std::iota(a.begin(), a.end(), 0);
    do { CheckLis(a); } while (std::next_permutation(a.begin(), a.end()));
  }
  // Exhaust all length-seven arrays over three values, including ties.
  for (int mask = 0; mask < 2187; ++mask) {
    int value = mask;
    std::vector<int> a(7);
    for (int& x : a) { x = value % 3 - 1; value /= 3; }
    CheckLis(a);
  }
  Require(mo::ApproximateRangeLis({1, 2}, {}).empty(), "No LIS queries");
  Require(mo::RangeFrequencies({1, 2}, {}).empty(), "No frequency queries");
  Require(mo::RangeKth({1, 2}, {}).empty(), "No kth queries");
  CheckLargeCounts();
  std::cout << "Mo use-cases: differential and exhaustive tests passed\n";
}
