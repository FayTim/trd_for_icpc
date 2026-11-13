#include "segment_tree.h"
#include "range_add_min.h"
#include "frequency_tree.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace {

void Require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void TestBoundaries() {
  Build({5, 3, 3, 1, 4});
  Require(Min(1, 3) == 3, "Range must exclude the right endpoint");
  Require(FirstBelow(3) == 3, "Search must skip equal values");
  Require(FirstBelow(0, 3, 3) == 5, "Bounded search must exclude the right endpoint");
  Require(FirstBelow(2, 5, 4) == 2, "Bounded search must include the left endpoint");
  Require(FirstBelow(4, 5, 5) == 4, "Bounded search must skip smaller values outside the range");
  Require(FirstBelow(1, 1, 100) == 5, "Empty search range has no answer");
  Set(3, 6);
  Require(FirstBelow(3) == 5, "Missing answer must return the input size");
  Require(FirstBelow(5) == 1, "Search must return the leftmost match");

  const int high = std::numeric_limits<int>::max();
  const int low = std::numeric_limits<int>::min();
  Build({high});
  Require(Min(0, 1) == high, "INT_MAX must be a valid element");
  Require(FirstBelow(high) == 1, "Rebuilding must clear the previous tree");
  Require(FirstBelow(0, 1, high) == 1, "Bounded search must skip equal INT_MAX");
  Set(0, low);
  Require(Min(0, 1) == low, "Single-element assignment failed");
  Require(FirstBelow(high) == 0, "Search missed index zero");
  Require(FirstBelow(0, 1, high) == 0, "Bounded search missed INT_MIN");
  Require(FirstBelow(0, 1, low) == 1, "Bounded comparison must be strict");

  Build({});
  Require(Min(0, 0) == high, "Empty range must return INT_MAX");
  Require(FirstBelow(high) == 0, "An empty tree has no matching index");
  Require(FirstBelow(0, 0, high) == 0, "Empty bounded search failed");
}

void TestAgainstScan() {
  std::mt19937 random(314159);
  std::uniform_int_distribution<int> value(-10, 10);
  for (int size = 1; size <= 40; ++size) {
    std::vector<int> values(size);
    for (int& element : values) {
      element = value(random);
    }
    Build(values);
    std::uniform_int_distribution<int> index(0, size - 1);
    std::uniform_int_distribution<int> boundary(0, size);
    for (int trial = 0; trial < 200; ++trial) {
      const auto changed = index(random);
      values[changed] = value(random);
      Set(changed, values[changed]);

      auto left = boundary(random);
      auto right = boundary(random);
      if (left > right) {
        std::swap(left, right);
      }
      int expected = std::numeric_limits<int>::max();
      for (auto i = left; i < right; ++i) {
        expected = std::min(expected, values[i]);
      }
      Require(Min(left, right) == expected, "Range minimum differs from scan");

      const int threshold = value(random);
      const auto first = std::find_if(values.begin(), values.end(),
                                      [threshold](int x) { return x < threshold; });
      Require(FirstBelow(threshold) == first - values.begin(),
              "First index below threshold differs from scan");
      const auto bounded = std::find_if(values.begin() + left, values.begin() + right,
                                       [threshold](int x) { return x < threshold; });
      const int expected_index = bounded == values.begin() + right
                                     ? size : static_cast<int>(bounded - values.begin());
      Require(FirstBelow(left, right, threshold) == expected_index,
              "Bounded first index differs from scan");
    }
  }
}

void TestFrequencyBoundaries() {
  namespace ft = frequency_tree;
  ft::Build({});
  Require(ft::Kth(1) == 0 && ft::Kth(0) == 0 && ft::Kth(-1) == 0,
          "Empty frequency tree must reject all ranks");
  ft::Build({0, 0, 0});
  Require(ft::Kth(1) == 3, "Zero counts must have no rank");
  ft::Set(1, 2);
  Require(ft::Kth(1) == 1 && ft::Kth(2) == 1 && ft::Kth(3) == 3,
          "Repeated values must occupy consecutive ranks");
  ft::Set(1, 0);
  Require(ft::Kth(1) == 3, "Setting a frequency to zero must remove its ranks");

  ft::Build({3000000000LL, 0, 4000000000LL});
  Require(ft::Kth(3000000000LL) == 0 && ft::Kth(3000000001LL) == 2,
          "Rank selection must support totals greater than INT_MAX");
  Require(ft::Kth(7000000000LL) == 2 && ft::Kth(7000000001LL) == 3,
          "Final rank boundary is incorrect");
  Require(ft::Kth(0) == 3 && ft::Kth(-5) == 3, "Nonpositive ranks must return n");
  ft::Set(0, 0);
  Require(ft::Kth(1) == 2, "Leading zero counts must be skipped");

  const long long high = std::numeric_limits<long long>::max();
  ft::Build({high - 5, 5});
  Require(ft::Kth(high - 5) == 0 && ft::Kth(high - 4) == 1 && ft::Kth(high) == 1,
          "LLONG_MAX total must be supported");
  Require(ft::Kth(std::numeric_limits<long long>::min()) == 2,
          "LLONG_MIN is an invalid rank");
}

void TestFrequencyAgainstScan() {
  namespace ft = frequency_tree;
  std::mt19937 random(161803);
  std::uniform_int_distribution<int> frequency(0, 20);
  for (int size = 1; size <= 40; ++size) {
    std::vector<long long> counts(size);
    for (auto& count : counts) count = frequency(random);
    ft::Build(counts);
    std::uniform_int_distribution<int> index(0, size - 1);
    for (int trial = 0; trial < 200; ++trial) {
      const int changed = index(random);
      counts[changed] = frequency(random);
      ft::Set(changed, counts[changed]);
      long long total = 0;
      for (long long count : counts) total += count;
      std::uniform_int_distribution<long long> rank(-1, total + 1);
      const long long k = rank(random);
      int expected = size;
      if (k > 0) {
        long long prefix = 0;
        for (int i = 0; i < size; ++i) {
          prefix += counts[i];
          if (prefix >= k) {
            expected = i;
            break;
          }
        }
      }
      Require(ft::Kth(k) == expected, "Rank selection differs from prefix scan");
      Require(ft::Kth(0) == size && ft::Kth(total + 1) == size,
              "Rank outside total must return n");
    }
  }
}

void TestRangeAddAgainstScan() {
  namespace range = range_add_min;
  range::Build({});
  range::Update(1, 0, 0, 0, 0, 5);
  Require(range::Min(1, 0, 0, 0, 0) == range::INF, "Empty tree query failed");

  std::mt19937 random(271828);
  std::uniform_int_distribution<int> delta(-10, 10);
  for (int size = 1; size <= 40; ++size) {
    std::vector<long long> values(size, 0);
    range::Build(values);
    range::Update(1, 0, size, 0, size, 3);
    range::Update(1, 0, size, 0, size, -1);
    std::fill(values.begin(), values.end(), 2);
    Require(range::Min(1, 0, size, size - 1, size) == 2,
            "Stacked updates must reach a leaf");

    std::uniform_int_distribution<int> boundary(0, size);
    for (int trial = 0; trial < 200; ++trial) {
      int l = boundary(random);
      int r = boundary(random);
      if (l > r) {
        std::swap(l, r);
      }
      if (trial % 3 != 0) {
        long long x = delta(random);
        range::Update(1, 0, size, l, r, x);
        for (int i = l; i < r; ++i) {
          values[i] += x;
        }
      } else {
        auto expected = std::min_element(values.begin() + l, values.begin() + r);
        Require(range::Min(1, 0, size, l, r) ==
                    (l == r ? range::INF : *expected),
                "Minimum after range additions differs from scan");
      }
    }
  }
}

}  // namespace

int main() {
  try {
    TestBoundaries();
    TestAgainstScan();
    TestRangeAddAgainstScan();
    TestFrequencyBoundaries();
    TestFrequencyAgainstScan();
  } catch (const std::exception& error) {
    std::cerr << "SegmentTree test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "SegmentTree tests passed\n";
}
