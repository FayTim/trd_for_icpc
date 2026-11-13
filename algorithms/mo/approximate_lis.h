#pragma once

#include "mo.h"

#include <algorithm>
#include <functional>
#include <vector>

namespace mo {

// Offline divide and conquer, NOT a Mo add/remove state.
// Source idea: ICPC 2025 Northwestern, I, Infection Investigation:
// https://nerc.itmo.ru/archive/2025/northern/tutorial.pdf (page 6).
// Strict LIS on [l,r); arbitrary int values, duplicates allowed.
// Empty intervals give 0. Otherwise 2*LIS <= 3*answer and
// 2*answer <= 3*LIS; the answer need not be a realizable subsequence length.
// Time O(n log^2(n+1) + q log(n+1)), extra memory O(n+q).
inline std::vector<int> ApproximateRangeLis(
    const std::vector<int>& a, const std::vector<Query>& queries) {
  int n = a.size();
  std::vector<int> answer(queries.size()), ids;
  for (int id = 0; id < int(queries.size()); ++id)
    if (queries[id].l < queries[id].r) ids.push_back(id);
  std::vector<int> suffix(n), prefix(n), ascents(n + 1);
  for (int i = 0; i < n; ++i)
    ascents[i + 1] = ascents[i] + (i > 0 && a[i - 1] < a[i]);

  auto solve = [&](auto&& self, int l, int r, int begin, int end) -> void {
    if (begin == end) return;
    if (r - l == 1) {
      for (int j = begin; j < end; ++j) answer[ids[j]] = 1;
      return;
    }
    int mid = l + (r - l) / 2;
    auto first_right = std::partition(ids.begin() + begin, ids.begin() + end,
        [&](int id) { return queries[id].r <= mid; });
    auto first_cross = std::partition(first_right, ids.begin() + end,
        [&](int id) { return queries[id].l >= mid; });
    int left_end = first_right - ids.begin();
    int right_end = first_cross - ids.begin();
    if (right_end < end) {
      std::vector<int> tails;
      // A decreasing subsequence in reverse order is an increasing one
      // in forward order. No negation (safe even for INT_MIN).
      for (int i = mid - 1; i >= l; --i) {
        auto it = std::lower_bound(tails.begin(), tails.end(), a[i],
                                   std::greater<int>());
        if (it == tails.end()) tails.push_back(a[i]);
        else *it = a[i];
        suffix[i] = tails.size();
      }
      tails.clear();
      for (int i = mid; i < r; ++i) {
        auto it = std::lower_bound(tails.begin(), tails.end(), a[i]);
        if (it == tails.end()) tails.push_back(a[i]);
        else *it = a[i];
        prefix[i] = tails.size();
      }
      for (int j = right_end; j < end; ++j) {
        int id = ids[j];
        auto q = queries[id];
        int x = suffix[q.l], y = prefix[q.r - 1];
        if (x == 1 && y == 1) {
          answer[id] = 1 + (ascents[q.r] > ascents[q.l + 1]);
        } else {
          answer[id] = std::max(x, y) + std::min(x, y) / 2;
        }
      }
    }
    self(self, l, mid, begin, left_end);
    self(self, mid, r, left_end, right_end);
  };
  solve(solve, 0, n, 0, ids.size());
  return answer;
}

}  // namespace mo
