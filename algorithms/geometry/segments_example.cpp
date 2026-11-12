#include "segments.h"

#include <iostream>

int main() {
  using namespace geometry;
  const Point a{0, 0}, b{3, 3}, c{0, 3}, d{3, 0};
  std::cout << SegmentsIntersect(a, b, c, d) << '\n';  // 1
  const auto p = IntersectLines(a, b, c, d);
  if (p.kind == LineKind::Unique) {
    // Exact answer: both coordinates are 27/18 = 3/2.
    std::cout << ToString(p.x_num) << '/' << ToString(p.den)
              << ' ' << ToString(p.y_num) << '/'
              << ToString(p.den) << '\n';
  }
  std::cout << IntegerPoint(p).has_value() << '\n';  // 0
  const auto q = IntersectLines({0, 0}, {2, 2}, {0, 2}, {2, 0});
  if (const auto integer = IntegerPoint(q)) {
    std::cout << integer->x << ' ' << integer->y << '\n';  // 1 1
  }
  std::cout << DistanceToSegment({2, 2}, {0, 0}, {4, 0})
            << '\n';  // 2
}
