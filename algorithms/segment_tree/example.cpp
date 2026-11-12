#include "segment_tree.h"

#include <iostream>

int main() {
  Build({5, 3, 3, 1, 4});
  std::cout << Min(1, 3) << '\n';     // 3
  std::cout << FirstBelow(3) << '\n'; // 3
  Set(3, 6);
  std::cout << FirstBelow(3) << '\n'; // 5 (not found)
}
