#pragma once

#include <vector>

class FenwickTree {
 public:
  explicit FenwickTree(int n) : n_(n), tree_(n) {}

  void Add(int i, long long delta) {
    for (; i < n_; i |= i + 1) tree_[i] += delta;
  }

  long long PrefixSum(int r) const {
    long long sum = 0;
    for (; r > 0; r &= r - 1) sum += tree_[r - 1];
    return sum;
  }

  long long Sum(int l, int r) const {
    return PrefixSum(r) - PrefixSum(l);
  }

 private:
  int n_;
  std::vector<long long> tree_;
};
