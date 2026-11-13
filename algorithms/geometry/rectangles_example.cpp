#include "rectangles.h"

#include <iostream>

int main() {
  using namespace geometry;
  const Rect a = MakeRect({4, 3}, {0, 0});
  const Rect b = MakeRect({2, -1}, {6, 2});
  if (const auto common = Intersect(a, b)) {
    std::cout << ToString(RectangleArea(*common)) << '\n';  // 4
  }
  const auto box = BoundingBox({{-2, 1}, {4, 3}, {1, -1}});
  std::cout << Perimeter(*box) << '\n';                    // 20
  std::cout << Contains(*box, {4, 3}) << '\n';              // 1
  const auto touch = Intersect(a, MakeRect({4, 1}, {6, 2}));
  std::cout << touch.has_value() << '\n';                  // 1
  std::cout << ToString(RectangleArea(*touch)) << '\n';     // 0
}
