#include "compress_coordinates.h"

#include <iostream>
#include <vector>

int main() {
  const std::vector<int> coords{50, -10, 50, 20};
  const auto [to, from, compressed] = CompressCoordinates(coords);

  // compressed: {2, 0, 2, 1}; from: {-10, 20, 50}.
  std::cout << compressed.at(0) << '\n';  // 2: index of the first input.
  std::cout << to.at(20) << '\n';         // 1: coordinate -> index.
  std::cout << from.at(2) << '\n';        // 50: index -> coordinate.
}
