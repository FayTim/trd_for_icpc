#pragma once

#include "mo.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace mo {

// Ranks preserve order; values[rank] restores the original integer.
struct CompressedArray {
  std::vector<int> ranks, values;
  explicit CompressedArray(const std::vector<int>& a) : values(a) {
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    for (int x : a) {
      ranks.push_back(std::lower_bound(values.begin(), values.end(), x) -
                      values.begin());
    }
  }
};

struct FrequencySummary {
  int distinct = 0;
  long long equal_pairs = 0, squared_frequencies = 0;
  bool operator==(const FrequencySummary&) const = default;
};

struct FrequencyState {
  const std::vector<int>& a;  // Compressed ranks.
  std::vector<int> count;
  FrequencySummary result;
  FrequencyState(const std::vector<int>& ranks, int sigma)
      : a(ranks), count(sigma) {}
  void AddRight(int i) {
    int& c = count[a[i]];
    result.distinct += (c == 0);
    result.equal_pairs += c;
    result.squared_frequencies += 2LL * c + 1;
    ++c;
  }
  void RemoveRight(int i) {
    int& c = count[a[i]];
    --c;
    result.distinct -= (c == 0);
    result.equal_pairs -= c;
    result.squared_frequencies -= 2LL * c + 1;
  }
  void AddLeft(int i) { AddRight(i); }
  void RemoveLeft(int i) { RemoveRight(i); }
  FrequencySummary Answer(int) const { return result; }
};

inline auto RangeFrequencies(const std::vector<int>& a,
                             const std::vector<Query>& queries) {
  CompressedArray compressed(a);
  FrequencyState state(compressed.ranks, compressed.values.size());
  return Solve(a.size(), queries, state);
}

// Value buckets as in Algorithmica: O(1) update, O(sqrt(sigma)) kth.
// https://ru.algorithmica.org/cs/decomposition/mo/
// Nonnegative frequencies. Kth(k) returns a rank, k is ONE-based.
// Precondition for Kth: 1 <= k <= the total frequency.
struct SqrtFrequencies {
  int block;
  std::vector<int> count, sum;
  explicit SqrtFrequencies(int sigma)
      : block(std::max(1, int(std::sqrt(sigma)))), count(sigma),
        sum((sigma + block - 1) / block) {}
  void Add(int rank, int delta) {
    count[rank] += delta;
    sum[rank / block] += delta;
  }
  int Kth(int k) const {
    int b = 0;
    while (k > sum[b]) k -= sum[b++];
    int rank = b * block;
    while (k > count[rank]) k -= count[rank++];
    return rank;
  }
};

struct KthQuery { int l, r, k; };  // [l,r), 1 <= k <= r-l.

struct KthState {
  const CompressedArray& a;
  const std::vector<KthQuery>& queries;
  SqrtFrequencies frequencies;
  KthState(const CompressedArray& array, const std::vector<KthQuery>& q)
      : a(array), queries(q), frequencies(array.values.size()) {}
  void AddLeft(int i) { frequencies.Add(a.ranks[i], 1); }
  void AddRight(int i) { AddLeft(i); }
  void RemoveLeft(int i) { frequencies.Add(a.ranks[i], -1); }
  void RemoveRight(int i) { RemoveLeft(i); }
  int Answer(int id) const {
    return a.values[frequencies.Kth(queries[id].k)];
  }
};

inline auto RangeKth(const std::vector<int>& a,
                     const std::vector<KthQuery>& queries) {
  CompressedArray compressed(a);
  KthState state(compressed, queries);
  std::vector<Query> intervals;
  for (auto q : queries) intervals.push_back({q.l, q.r});
  return Solve(a.size(), intervals, state);
}

}  // namespace mo
