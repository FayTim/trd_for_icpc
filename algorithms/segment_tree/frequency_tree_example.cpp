#include "frequency_tree.h"

#include <iostream>
#include <vector>

int main() {
  namespace ft = frequency_tree;
  const std::vector<int> values{-10, 20, 50}; // sorted distinct values
  ft::Build({2, 0, 3}); // multiset: {-10, -10, 50, 50, 50}
  std::cout << values[ft::Kth(1)] << '\n'; // -10
  std::cout << values[ft::Kth(3)] << '\n'; // 50

  const int missing = ft::Kth(6);
  if (missing == static_cast<int>(values.size())) {
    std::cout << "No such rank\n";
  }

  ft::Set(1, 1); // now there is one occurrence of 20
  std::cout << values[ft::Kth(3)] << '\n'; // 20
}
