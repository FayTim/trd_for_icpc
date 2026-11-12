#include "tree_independent_set.h"

#include <iostream>

int main() {
  std::cout << TreeIndependentSetWeight({{1}, {0, 2}, {1}}, {2, 3, 2})
            << '\n';  // 4
}
