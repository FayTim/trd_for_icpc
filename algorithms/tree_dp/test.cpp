#include "tree_independent_set.h"

#include <stdexcept>

void Check(const std::vector<std::vector<int>>& tree,
           const std::vector<int>& weight, long long expected) {
  if (TreeIndependentSetWeight(tree, weight) != expected) {
    throw std::runtime_error("Unexpected tree independent set weight");
  }
}

int main() {
  Check({}, {}, 0);
  Check({{}}, {5}, 5);
  Check({{1}, {0, 2}, {1}}, {-2, -3, -2}, 0);
  Check({{1}, {0, 2}, {1}}, {2, 3, 2}, 4);
  Check({{1, 2, 3}, {0}, {0}, {0}}, {10, 2, 3, 4}, 10);
  Check({{1, 2}, {0, 3}, {0}, {1}}, {3, 5, 4, 6}, 10);
  Check({{1}, {0, 2}, {1}}, {1500000000, 1, 1500000000}, 3000000000LL);
}
