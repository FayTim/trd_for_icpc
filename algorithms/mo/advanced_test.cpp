#include "rollback_mo.h"

#include <climits>
#include <deque>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>

namespace {

void Check(bool ok, const char* message) {
  if (!ok) throw std::runtime_error(message);
}

struct OrderedRollback {
  std::deque<int> current;
  std::vector<bool> from_left;

  void AddLeft(int i) {
    Check(current.empty() || current.front() == i + 1, "Bad prepend");
    current.push_front(i);
    from_left.push_back(true);
  }
  void AddRight(int i) {
    Check(current.empty() || current.back() + 1 == i, "Bad append");
    current.push_back(i);
    from_left.push_back(false);
  }
  std::size_t Snapshot() const { return from_left.size(); }
  void Rollback(std::size_t saved) {
    Check(saved <= from_left.size(), "Cannot roll forward");
    while (from_left.size() > saved) {
      if (from_left.back()) current.pop_front();
      else current.pop_back();
      from_left.pop_back();
    }
  }
  std::vector<int> Answer(int) const {
    return {current.begin(), current.end()};
  }
};

void TestRollback() {
  std::mt19937 rng(0x72a41);
  Check(mo::RollbackModeFrequency({}, {}).empty(), "Empty query list");
  Check(mo::RollbackModeFrequency({}, {{0, 0}}) == std::vector<int>{0},
        "Empty array");
  Check(mo::RollbackModeFrequency({INT_MIN, INT_MAX, INT_MIN}, {{0, 3}})
            == std::vector<int>{2}, "Extreme values compression");
  for (int trial = 0; trial < 180; ++trial) {
    const int n = 1 + rng() % 45;
    std::vector<int> a(n);
    for (int& x : a) x = static_cast<int>(rng() % 11) - 5;
    std::vector<mo::Query> queries;
    for (int l = 0; l <= n; ++l)
      for (int r = l; r <= n; ++r) queries.push_back({l, r});
    queries.push_back({0, n});
    queries.push_back({n, n});
    std::shuffle(queries.begin(), queries.end(), rng);
    std::vector<int> expected;
    std::vector<std::vector<int>> ordered;
    for (auto [l, r] : queries) {
      std::map<int, int> frequency;
      int maximum = 0;
      for (int i = l; i < r; ++i)
        maximum = std::max(maximum, ++frequency[a[i]]);
      expected.push_back(maximum);
      std::vector<int> interval(r - l);
      std::iota(interval.begin(), interval.end(), l);
      ordered.push_back(interval);
    }
    for (int block : {0, 1, 2, 7, n, n + 10}) {
      Check(mo::RollbackModeFrequency(a, queries, block) == expected,
            "Rollback mode differs from brute force");
      OrderedRollback state;
      Check(mo::SolveRollback(n, queries, state, block) == ordered,
            "Rollback interval or side mismatch");
      Check(state.current.empty() && state.from_left.empty(),
            "Rollback runner did not restore initial state");
    }
  }
}

}  // namespace

int main() {
  try {
    TestRollback();
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
