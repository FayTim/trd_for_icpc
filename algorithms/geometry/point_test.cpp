#include <cstdlib>
#include <iostream>
#include <limits>

#include "point.h"

void Check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << message << '\n';
    std::exit(1);
  }
}

int main() {
  using namespace geometry;
  for (long long x = -20; x <= 20; ++x) {
    for (long long y = -20; y <= 20; ++y) {
      const Point v{x, y}, w = Perp(v);
      Check(Dot(v, w) == 0, "perpendicular rotation");
      Check(Distance2({}, v) == Distance2({}, w), "rotation length");
      Check(Cross(v, w) == Dot(v, v), "square area");
      Check(Perp(Perp(Perp(w))) == v, "four rotations");
      const Point primitive = PrimitiveDirection(v);
      Check(Cross(v, primitive) == 0, "primitive is parallel");
      if (v != Point{}) {
        Check(std::gcd(primitive.x, primitive.y) == 1, "primitive gcd");
        Check(Dot(v, primitive) > 0, "primitive preserves direction");
      }
      Check(LineDirection(v) == LineDirection({-x, -y}),
            "undirected normalization");
      Check(PrimitiveDirection(primitive) == primitive, "idempotence");
    }
  }
  const Point a{-1000000000, -1000000000};
  const Point b{1000000000, -1000000000};
  const Point c{-1000000000, 1000000000};
  Check(Orient(a, b, c) == Wide(4000000000LL) * 1000000000,
        "large orientation");
  Check(Sign(Orient(a, c, b)) == -1, "clockwise orientation");
  Check(Orient(a, b, {0, -1000000000}) == 0, "collinearity");
  Check(Distance2(a, {1000000000, 1000000000}) ==
            Wide(8000000000LL) * 1000000000, "large distance");
  Check(ToString(0) == "0", "print zero");
  Check(ToString(-123456789) == "-123456789", "print negative");
  Check(ToString(Wide(10000000000LL) * 10000000000LL) ==
            "100000000000000000000", "print beyond int64");
  Check(ToString(std::numeric_limits<Wide>::min()) ==
            "-170141183460469231731687303715884105728", "print int128 min");
  std::cout << "point tests passed\n";
}
