#pragma once

#include <algorithm>
#include <limits>
#include <vector>

inline constexpr int INF = std::numeric_limits<int>::max();
inline int n, base;
inline std::vector<int> tree;

inline void Build(const std::vector<int>& a) {
  n = static_cast<int>(a.size());
  base = 1;
  while (base < n) {
    base *= 2;
  }
  tree.assign(2 * base, INF);
  std::copy(a.begin(), a.end(), tree.begin() + base);
  for (int v = base - 1; v > 0; --v) {
    tree[v] = std::min(tree[2 * v], tree[2 * v + 1]);
  }
}

inline void Set(int i, int x) {
  i += base;
  tree[i] = x;
  for (i /= 2; i > 0; i /= 2) {
    tree[i] = std::min(tree[2 * i], tree[2 * i + 1]);
  }
}

inline int Min(int l, int r) {
  int ans = INF;
  for (l += base, r += base; l < r; l /= 2, r /= 2) {
    if (l % 2 != 0) {
      ans = std::min(ans, tree[l++]);
    }
    if (r % 2 != 0) {
      ans = std::min(ans, tree[--r]);
    }
  }
  return ans;
}

inline int FirstBelow(int x) {
  if (tree[1] >= x) {
    return n;
  }
  int v = 1;
  while (v < base) {
    v *= 2;
    if (tree[v] >= x) {
      ++v;
    }
  }
  return v - base;
}

namespace segment_tree_detail {
inline int FirstBelow(int v, int lo, int hi, int l, int r, int x) {
  if (r <= lo || hi <= l || tree[v] >= x) {
    return n;
  }
  if (hi - lo == 1) {
    return lo;
  }
  const int mid = lo + (hi - lo) / 2;
  const int left = FirstBelow(2 * v, lo, mid, l, r, x);
  return left != n ? left : FirstBelow(2 * v + 1, mid, hi, l, r, x);
}
}  // namespace segment_tree_detail

// Первый индекс i в [l, r) с a[i] < x; n, если такого индекса нет.
// Требуется 0 <= l <= r <= n.
inline int FirstBelow(int l, int r, int x) {
  if (l == r) {
    return n;
  }
  return segment_tree_detail::FirstBelow(1, 0, base, l, r, x);
}
