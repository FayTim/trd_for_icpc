#include <iostream>

#include "point.h"

int main() {
  using namespace geometry;
  const Point p{0, 0}, v{3, 4}, w = Perp(v);
  // A square of area 25: p, p + v, p + v + w, p + w.
  for (const Point q : {p, p + v, p + v + w, p + w}) {
    std::cout << q.x << ' ' << q.y << '\n';
  }
  std::cout << ToString(Dot(v, w)) << '\n';  // 0: perpendicular.
  std::cout << ToString(Distance2(p, v)) << '\n';  // 25.
  const Point direction = LineDirection({-6, -8});
  std::cout << direction.x << ' ' << direction.y << '\n';  // 3 4.
}
