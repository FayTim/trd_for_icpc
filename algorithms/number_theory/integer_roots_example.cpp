#include "integer_roots.h"

#include <iostream>

int main() {
  std::cout << nt::FloorKthRoot(80, 3) << '\n';  // 4: 4^3 <= 80 < 5^3.
  std::cout << nt::FloorKthRoot(81, 2) << '\n';  // 9: exact square.
  std::cout << nt::FloorKthRoot(82, 2) << '\n';  // 9: round down.
}
