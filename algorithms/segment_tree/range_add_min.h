#pragma once

#include <algorithm>
#include <limits>
#include <optional>
#include <vector>

namespace range_add_min {
inline constexpr long long INF = std::numeric_limits<long long>::max();
inline std::vector<long long> mn;
inline std::vector<std::optional<long long>> lazy;

inline void Build(const std::vector<long long>& a, int v, int lo, int hi) {
  if (hi - lo == 1) {
    mn[v] = a[lo];
    return;
  }
  int mid = lo + (hi - lo) / 2;
  Build(a, 2 * v, lo, mid);
  Build(a, 2 * v + 1, mid, hi);
  mn[v] = std::min(mn[2 * v], mn[2 * v + 1]);
}

inline void Build(const std::vector<long long>& a) {
  mn.assign(4 * a.size(), 0);
  lazy.assign(4 * a.size(), std::nullopt);
  if (!a.empty()) {
    Build(a, 1, 0, static_cast<int>(a.size()));
  }
}

inline void Apply(int v, long long x) {
  mn[v] += x;
  lazy[v] = lazy[v].value_or(0) + x;
}

inline void Push(int v) {
  if (lazy[v]) {
    Apply(2 * v, *lazy[v]);
    Apply(2 * v + 1, *lazy[v]);
    lazy[v].reset();
  }
}

inline void Update(int v, int lo, int hi, int l, int r, long long x) {
  if (l == r || r <= lo || hi <= l) {
    return;
  }
  if (l <= lo && hi <= r) {
    Apply(v, x);
    return;
  }
  Push(v);
  int mid = lo + (hi - lo) / 2;
  Update(2 * v, lo, mid, l, r, x);
  Update(2 * v + 1, mid, hi, l, r, x);
  mn[v] = std::min(mn[2 * v], mn[2 * v + 1]);
}

inline long long Min(int v, int lo, int hi, int l, int r) {
  if (l == r || r <= lo || hi <= l) {
    return INF;
  }
  if (l <= lo && hi <= r) {
    return mn[v];
  }
  Push(v);
  int mid = lo + (hi - lo) / 2;
  return std::min(Min(2 * v, lo, mid, l, r),
                  Min(2 * v + 1, mid, hi, l, r));
}
}  // namespace range_add_min
