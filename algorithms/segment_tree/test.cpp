#include "segment_tree.h"
#include "range_add_min.h"

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
  Set(3, 6);
  Require(FirstBelow(3) == 5, "Missing answer must return the input size");
  Require(FirstBelow(5) == 1, "Search must return the leftmost match");

  const int high = std::numeric_limits<int>::max();
  const int low = std::numeric_limits<int>::min();
  Build({high});
  Require(Min(0, 1) == high, "INT_MAX must be a valid element");
  Require(FirstBelow(high) == 1, "Rebuilding must clear the previous tree");
  Set(0, low);
  Require(Min(0, 1) == low, "Single-element assignment failed");
  Require(FirstBelow(high) == 0, "Search missed index zero");

  Build({});
  Require(Min(0, 0) == high, "Empty range must return INT_MAX");
  Require(FirstBelow(high) == 0, "An empty tree has no matching index");
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
  } catch (const std::exception& error) {
    std::cerr << "SegmentTree test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "SegmentTree tests passed\n";
}
