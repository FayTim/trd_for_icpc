#include "merge_piles.h"

#include <stdexcept>

void Check(const std::vector<int>& piles, long long expected) {
  if (MergePilesCost(piles) != expected) {
    throw std::runtime_error("Unexpected pile merge cost");
  }
}

int main() {
  Check({}, 0);
  Check({7}, 0);
  Check({0, 0, 0}, 0);
  Check({7, 9}, 16);
  Check({3, 3, 4}, 16);
  Check({4, 3, 3, 4}, 28);
  Check({100, 1, 1, 1}, 108);
  Check({1500000000, 1500000000}, 3000000000LL);
}
