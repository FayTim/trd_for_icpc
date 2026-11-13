#include "mo.h"

#include <array>
#include <deque>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

struct Answer {
  int id;
  std::vector<long long> values;
  long long sum;
  bool operator==(const Answer&) const = default;
};

// Verifies the *direction* of every callback, not just the final multiset.
struct Window {
  std::vector<long long> a;
  std::deque<int> indices;
  int l = 0, r = 0;
  long long sum = 0;
  std::array<int, 4> calls{};

  void AddLeft(int i) {
    Require(i == l - 1, "AddLeft must prepend the adjacent index");
    --l;
    indices.push_front(i);
    sum += a[i];
    ++calls[0];
  }
  void AddRight(int i) {
    Require(i == r, "AddRight must append the adjacent index");
    ++r;
    indices.push_back(i);
    sum += a[i];
    ++calls[1];
  }
  void RemoveLeft(int i) {
    Require(!indices.empty() && i == l && indices.front() == i,
            "RemoveLeft must remove an existing first element");
    ++l;
    indices.pop_front();
    sum -= a[i];
    ++calls[2];
  }
  void RemoveRight(int i) {
    Require(!indices.empty() && i == r - 1 && indices.back() == i,
            "RemoveRight must remove an existing last element");
    --r;
    indices.pop_back();
    sum -= a[i];
    ++calls[3];
  }
  ::Answer Answer(int id) const {
    std::vector<long long> values;
    for (int i : indices) values.push_back(a[i]);
    return {id, values, sum};
  }
};

Answer Brute(const std::vector<long long>& a, int l, int r, int id) {
  return {id, {a.begin() + l, a.begin() + r},
          std::accumulate(a.begin() + l, a.begin() + r, 0LL)};
}

void TestStatic(std::mt19937& rng) {
  std::array<int, 4> calls{};
  for (int iteration = 0; iteration < 300; ++iteration) {
    const int n = iteration < 3 ? iteration : rng() % 45;
    std::vector<long long> a(n);
    for (auto& x : a) x = static_cast<int>(rng() % 17) - 8;
    std::vector<mo::Query> queries{{0, 0}, {n, n}, {0, n}, {0, n}};
    for (int l = 0; l <= n; ++l)
      for (int r = l; r <= n; ++r) queries.push_back({l, r});
    std::shuffle(queries.begin(), queries.end(), rng);
    std::vector<Answer> expected;
    for (int id = 0; id < static_cast<int>(queries.size()); ++id)
      expected.push_back(Brute(a, queries[id].l, queries[id].r, id));
    for (int block : {0, 1, 3, n + 1}) {
      Window state;
      state.a = a;
      Require(mo::Solve(n, queries, state, block) == expected,
              "Static Mo differs from direct slice");
      for (int i = 0; i < 4; ++i) calls[i] += state.calls[i];
    }
  }
  for (int count : calls) Require(count > 0, "Missing boundary direction");
  Window empty;
  Require(mo::Solve(0, {}, empty).empty(), "No queries must give no answers");
  Require(empty.calls == std::array<int, 4>{}, "Empty run touched State");
}

struct Update { int pos; long long before, after; };

struct TimeWindow : Window {
  std::vector<Update> updates;
  int time = 0;
  // forward/backward crossed with inside/outside the current window.
  std::array<int, 4> time_calls{};

  void Apply(int id, bool forward, int left, int right) {
    Require(left == l && right == r, "Apply received stale boundaries");
    Require(id == (forward ? time : time - 1), "Nonadjacent time step");
    const auto u = updates[id];
    const long long before = forward ? u.before : u.after;
    const long long after = forward ? u.after : u.before;
    Require(a[u.pos] == before, "Undo must use the immediately previous value");
    const bool inside = l <= u.pos && u.pos < r;
    if (inside) sum += after - before;
    a[u.pos] = after;
    time += forward ? 1 : -1;
    ++time_calls[2 * forward + inside];
  }
};

void TestUpdates(std::mt19937& rng) {
  std::array<int, 4> time_calls{};
  for (int iteration = 0; iteration < 300; ++iteration) {
    const int n = 1 + rng() % 30;
    std::vector<long long> initial(n);
    for (auto& x : initial) x = static_cast<int>(rng() % 19) - 9;
    std::vector<std::vector<long long>> versions{initial};
    std::vector<Update> updates;
    for (int i = 0; i < 40; ++i) {
      int pos = i % 3 == 0 ? 0 : rng() % n;  // repeated same-position updates
      long long after = i % 5 == 0 ? versions.back()[pos]
          : static_cast<int>(rng() % 19) - 9;  // include no-op updates
      updates.push_back({pos, versions.back()[pos], after});
      auto next = versions.back();
      next[pos] = after;
      versions.push_back(next);
    }
    std::vector<mo::TimedQuery> queries;
    for (int i = 0; i < 200; ++i) {
      int l = rng() % (n + 1), r = rng() % (n + 1);
      if (l > r) std::swap(l, r);
      queries.push_back({l, r, static_cast<int>(rng() % versions.size())});
    }
    queries.insert(queries.end(), {{0, n, 40}, {0, n, 0}, {n, n, 40},
                                   {0, 0, 0}, {0, n, 40}});
    std::shuffle(queries.begin(), queries.end(), rng);
    std::vector<Answer> expected;
    for (int id = 0; id < static_cast<int>(queries.size()); ++id) {
      auto q = queries[id];
      expected.push_back(Brute(versions[q.t], q.l, q.r, id));
    }
    for (int block : {0, 1, 3, n + 1}) {
      TimeWindow state;
      state.a = initial;
      state.updates = updates;
      Require(mo::SolveWithUpdates(n, queries, updates.size(), state, block)
                  == expected, "3D Mo differs from explicit array versions");
      for (int i = 0; i < 4; ++i) time_calls[i] += state.time_calls[i];
    }
  }
  for (int count : time_calls) Require(count > 0, "Missing time direction");
  TimeWindow empty;
  Require(mo::SolveWithUpdates(0, {}, 0, empty).empty(), "Empty 3D queries");
  auto result = mo::SolveWithUpdates(0, {{0, 0, 0}}, 0, empty);
  Require(result == std::vector<Answer>{{0, {}, 0}}, "Empty 3D window");
  TimeWindow unchanged;
  unchanged.a = {8, -4, 8};
  Require(mo::SolveWithUpdates(3, {{0, 3, 0}, {1, 1, 0}}, 0, unchanged)
              == std::vector<Answer>{{0, {8, -4, 8}, 12}, {1, {}, 0}},
          "3D Mo with no updates");
}

int main() {
  try {
    std::mt19937 rng(0xA10C2025);
    TestStatic(rng);
    TestUpdates(rng);
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}
