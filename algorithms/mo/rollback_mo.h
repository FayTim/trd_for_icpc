#pragma once

#include "mo.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace mo {

// Half-open queries, 0 <= l <= r <= n; block_size >= 0 (0 = automatic).
// State starts empty and has AddLeft(i), AddRight(i), Answer(query_id),
// Snapshot(), Rollback(snapshot). Rollback must restore ALL state changes.
// No arbitrary removals are required, but BOTH kinds of insertion must be cheap.
// In particular, append-only LIS + its undo does not meet this contract.
// State is restored to its initial snapshot when the function returns.
template <class State>
auto SolveRollback(int n, const std::vector<Query>& queries, State& state,
                   int block_size = 0) {
  using Answer = decltype(state.Answer(0));
  std::vector<Answer> answers(queries.size());
  if (queries.empty()) return answers;
  const int block = block_size > 0 ? block_size
      : std::max(1, static_cast<int>(n / std::sqrt(queries.size())));
  const auto empty = state.Snapshot();
  std::vector<int> order;
  for (int i = 0; i < static_cast<int>(queries.size()); ++i) {
    if (queries[i].l == queries[i].r) answers[i] = state.Answer(i);
    else order.push_back(i);
  }
  std::sort(order.begin(), order.end(), [&](int a, int b) {
    const int x = queries[a].l / block, y = queries[b].l / block;
    if (x != y) return x < y;
    return queries[a].r < queries[b].r;
  });
  int group = -1, boundary = 0, right = 0;
  for (int id : order) {
    const auto [l, r] = queries[id];
    if (l / block != group) {
      state.Rollback(empty);
      group = l / block;
      const int start = l - l % block;
      boundary = start + std::min(block, n - start);
      right = boundary;
    }
    if (r <= boundary) {
      // These come before long queries because r is nondecreasing.
      for (int i = l; i < r; ++i) state.AddRight(i);
      answers[id] = state.Answer(id);
      state.Rollback(empty);
    } else {
      while (right < r) state.AddRight(right++);
      const auto saved = state.Snapshot();
      for (int i = boundary; i > l;) state.AddLeft(--i);
      answers[id] = state.Answer(id);
      state.Rollback(saved);
    }
  }
  state.Rollback(empty);
  return answers;
}

// Maximum frequency on each interval; arbitrary int values, empty answer = 0.
// Saving the old maximum makes undo O(1), without a frequency-of-frequencies.
inline std::vector<int> RollbackModeFrequency(
    const std::vector<int>& values, const std::vector<Query>& queries,
    int block_size = 0) {
  auto sorted = values;
  std::sort(sorted.begin(), sorted.end());
  sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
  std::vector<int> rank;
  rank.reserve(values.size());
  for (int v : values)
    rank.push_back(static_cast<int>(
        std::lower_bound(sorted.begin(), sorted.end(), v) - sorted.begin()));
  struct State {
    const std::vector<int>& rank;
    std::vector<int> frequency;
    int maximum;
    std::vector<std::pair<int, int>> history;

    void AddRight(int i) {
      const int v = rank[i];
      history.emplace_back(v, maximum);
      maximum = std::max(maximum, ++frequency[v]);
    }
    void AddLeft(int i) { AddRight(i); }
    std::size_t Snapshot() const { return history.size(); }
    void Rollback(std::size_t saved) {
      while (history.size() > saved) {
        const auto [v, old_maximum] = history.back();
        history.pop_back();
        --frequency[v];
        maximum = old_maximum;
      }
    }
    int Answer(int) const { return maximum; }
  } state{rank, std::vector<int>(sorted.size()), 0, {}};
  return SolveRollback(static_cast<int>(values.size()), queries, state,
                       block_size);
}

}  // namespace mo
