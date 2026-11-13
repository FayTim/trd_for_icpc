#include "range_add_min.h"

#include <iostream>

int main() {
  range_add_min::Build({5, 3, 3, 1, 4});
  range_add_min::Update(1, 0, 5, 1, 4, 2);        // [5, 5, 5, 3, 4]
  std::cout << range_add_min::Min(1, 0, 5, 1, 4)  // 3
            << '\n';
}
