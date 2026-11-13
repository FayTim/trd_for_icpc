#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

// The half-open window and snake order follow the idea of KACTL's
// MoQueries.h (Simon Lindholm, CC0); this implementation is independent.
namespace mo {

struct Query { int l, r; };  // [l, r), 0 <= l <= r <= n

namespace detail {

template <class State>
void Move(int& l, int& r, int next_l, int next_r, State& state) {
  // Expand first: never remove an element outside the current window.
  while (l > next_l) state.AddLeft(--l);
  while (r < next_r) state.AddRight(r++);
  while (l < next_l) state.RemoveLeft(l++);
  while (r > next_r) state.RemoveRight(--r);
}

}  // namespace detail

// State starts with an empty window. Answer(id) returns a value, not a
// reference, is default-constructible and assignable, and preserves State.
// Answers follow input order; State stays at the last query.
// B=0 selects max(1, floor(n/sqrt(q))); an explicit B must be positive.
template <class State>
auto Solve(int n, const std::vector<Query>& queries, State& state,
           int block_size = 0) {
  std::vector<decltype(state.Answer(0))> answers(queries.size());
  if (queries.empty()) return answers;
  std::vector<int> ids(queries.size());
  std::iota(ids.begin(), ids.end(), 0);
  const int b = block_size > 0 ? block_size
      : std::max(1, static_cast<int>(n / std::sqrt(queries.size())));
  std::sort(ids.begin(), ids.end(), [&](int i, int j) {
    const int bi = queries[i].l / b, bj = queries[j].l / b;
    if (bi != bj) return bi < bj;
    return bi & 1 ? queries[i].r > queries[j].r
                  : queries[i].r < queries[j].r;
  });
  int l = 0, r = 0;
  for (int id : ids) {
    detail::Move(l, r, queries[id].l, queries[id].r, state);
    answers[id] = state.Answer(id);
  }
  return answers;
}

struct TimedQuery { int l, r, t; };  // t = number of preceding updates

// Same boundary callbacks as Solve. State starts empty at time 0.
// Apply(id, forward, l, r) changes one array position; if it is inside
// [l,r), update the statistic too. It must also support undo (forward=false).
// Preconditions: valid [l,r), 0 <= t <= update_count; all updates are
// known to State. Interior replacement must be cheap for this statistic.
// B~n^(2/3) is a default for n,q,update_count of the same order of magnitude.
template <class State>
auto SolveWithUpdates(int n, const std::vector<TimedQuery>& queries,
                      [[maybe_unused]] int update_count, State& state,
                      int block_size = 0) {
  std::vector<decltype(state.Answer(0))> answers(queries.size());
  if (queries.empty()) return answers;
  const int b = block_size > 0 ? block_size
      : std::max(1, static_cast<int>(std::pow(n, 2.0 / 3.0)));
  std::vector<int> ids(queries.size());
  std::iota(ids.begin(), ids.end(), 0);
  std::sort(ids.begin(), ids.end(), [&](int i, int j) {
    const int li = queries[i].l / b, lj = queries[j].l / b;
    if (li != lj) return li < lj;
    const int ri = queries[i].r / b, rj = queries[j].r / b;
    if (ri != rj) return li & 1 ? ri > rj : ri < rj;
    return (li ^ ri) & 1 ? queries[i].t > queries[j].t
                         : queries[i].t < queries[j].t;
  });
  int l = 0, r = 0, t = 0;
  for (int id : ids) {
    const auto q = queries[id];
    while (t < q.t) state.Apply(t++, true, l, r);
    while (t > q.t) state.Apply(--t, false, l, r);
    detail::Move(l, r, q.l, q.r, state);
    answers[id] = state.Answer(id);
  }
  return answers;
}

}  // namespace mo
