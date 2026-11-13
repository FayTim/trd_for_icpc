#pragma once

#include <algorithm>
#include <vector>

namespace frequency_tree {
inline int n, base;
inline std::vector<long long> tree;

// Частоты неотрицательны, их сумма помещается в long long.
// 2 * base должно помещаться в int, как у обычного дерева отрезков.
inline void Build(const std::vector<long long>& frequency) {
  n = static_cast<int>(frequency.size());
  base = 1;
  while (base < n) {
    base *= 2;
  }
  tree.assign(2 * base, 0);
  std::copy(frequency.begin(), frequency.end(), tree.begin() + base);
  for (int v = base - 1; v > 0; --v) {
    tree[v] = tree[2 * v] + tree[2 * v + 1];
  }
}

// Присваивает частоту, не прибавляет её. 0 <= i < n, count >= 0;
// новая суммарная частота также должна помещаться в long long.
inline void Set(int i, long long count) {
  i += base;
  tree[i] = count;
  for (i /= 2; i > 0; i /= 2) {
    tree[i] = tree[2 * i] + tree[2 * i + 1];
  }
}

// Индекс k-го элемента с учётом частот; k начинается с 1.
// Возвращает n при k <= 0 или k больше суммарной частоты.
inline int Kth(long long k) {
  if (k <= 0 || k > tree[1]) {
    return n;
  }
  int v = 1;
  while (v < base) {
    v *= 2;
    if (k > tree[v]) {
      k -= tree[v];
      ++v;
    }
  }
  return v - base;
}
}  // namespace frequency_tree
