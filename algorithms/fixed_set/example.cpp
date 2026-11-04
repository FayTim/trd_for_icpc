#include <iostream>
#include <vector>

#include "fixed_set.h"

int main() {
  FixedSet set;
  set.Initialize({7, -3, 7, 0});

  std::cout << std::boolalpha;
  std::cout << set.Contains(7) << '\n';   // true
  std::cout << set.Contains(42) << '\n';  // false
  std::cout << set.Contains(-3) << '\n';  // true

  set.Initialize({42});
  std::cout << set.Contains(7) << '\n';   // false
  std::cout << set.Contains(42) << '\n';  // true

  set.Initialize({});
  std::cout << set.Contains(42) << '\n';  // false
}
