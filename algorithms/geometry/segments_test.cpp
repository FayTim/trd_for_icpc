#include "segments.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

using namespace geometry;

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

void Near(long double actual, long double expected) {
  Require(std::abs(actual - expected) <= 1e-12L * (1 + expected),
          "Unexpected distance");
}

// On [-1, 1]^2 any nonparallel intersection parameter has a
// denominator at most 8. Enumerate rational witnesses on ab rather
// than reproducing the orientation-sign intersection algorithm.
bool ReferenceSegments(Point a, Point b, Point c, Point d) {
  for (int den = 1; den <= 8; ++den) {
    for (int k = 0; k <= den; ++k) {
      const long long x = a.x * den + (b.x - a.x) * k;
      const long long y = a.y * den + (b.y - a.y) * k;
      if ((x - c.x * den) * (d.y - c.y) !=
          (y - c.y * den) * (d.x - c.x)) continue;
      if (std::min(c.x, d.x) * den <= x &&
          x <= std::max(c.x, d.x) * den &&
          std::min(c.y, d.y) * den <= y &&
          y <= std::max(c.y, d.y) * den) return true;
    }
  }
  return false;
}

void TestExhaustiveSegments() {
  std::vector<Point> points;
  for (int x = -1; x <= 1; ++x)
    for (int y = -1; y <= 1; ++y) points.push_back({x, y});
  for (Point a : points) for (Point b : points)
    for (Point c : points) for (Point d : points) {
      Require(SegmentsIntersect(a, b, c, d) ==
                  ReferenceSegments(a, b, c, d),
              "Segment intersection disagrees with rational witnesses");
    }
  constexpr long long m = 1000000000;
  Require(SegmentsIntersect({-m, -m}, {m, m}, {-m, m}, {m, -m}),
          "Large crossing segments must intersect");
  Require(!SegmentsIntersect({-m, -m}, {m, m - 1}, {-m, -m + 1},
                             {m, m}),
          "Large parallel segments must not intersect");
}

void TestLineCases() {
  const auto half = IntersectLines({0, 0}, {1, 1}, {0, 1}, {1, 0});
  Require(half.kind == LineKind::Unique && half.den > 0 &&
              2 * half.x_num == half.den && 2 * half.y_num == half.den,
          "Half-integer intersection is wrong");
  Require(!IntegerPoint(half), "Half-integer intersection was truncated");
  const auto negative = IntersectLines({-2, 0}, {0, -2}, {-2, -2}, {0, 0});
  Require(IntegerPoint(negative) == Point{-1, -1},
          "Negative integer intersection is wrong");
  Require(IntersectLines({0, 0}, {1, 1}, {0, 1}, {1, 2}).kind ==
              LineKind::Parallel, "Parallel classification is wrong");
  Require(IntersectLines({0, 0}, {1, 1}, {3, 3}, {2, 2}).kind ==
              LineKind::Coincident, "Coincident classification is wrong");
  Require(IntersectLines({0, 0}, {0, 0}, {0, 0}, {1, 1}).kind ==
              LineKind::Degenerate, "First point-line must be degenerate");
  Require(IntersectLines({0, 0}, {1, 1}, {2, 2}, {2, 2}).kind ==
              LineKind::Degenerate, "Second point-line must be degenerate");
  Require(IntersectLines({0, 0}, {0, 0}, {1, 1}, {1, 1}).kind ==
              LineKind::Degenerate, "Two point-lines must be degenerate");
  Require(!IntegerPoint({LineKind::Parallel}),
          "Parallel lines must not have an integer intersection");
  const Wide lo = std::numeric_limits<long long>::min();
  const Wide hi = std::numeric_limits<long long>::max();
  Require(!IntegerPoint({LineKind::Unique, hi + 1, 0, 1}),
          "Out-of-range x coordinate was narrowed");
  Require(!IntegerPoint({LineKind::Unique, 0, lo - 1, 1}),
          "Out-of-range y coordinate was narrowed");
  Require(IntegerPoint({LineKind::Unique, lo, hi, 1}) ==
              Point{static_cast<long long>(lo), static_cast<long long>(hi)},
          "Long long boundary values were rejected");
}

void TestRandomLines() {
  std::mt19937_64 random(0x11E5);
  std::uniform_int_distribution<long long> coordinate(-1000000000,
                                                     1000000000);
  auto point = [&]() { return Point{coordinate(random), coordinate(random)}; };
  for (int trial = 0; trial < 10000; ++trial) {
    const Point a = point(), b = point(), c = point(), d = point();
    const auto p = IntersectLines(a, b, c, d);
    if (p.kind != LineKind::Unique) continue;
    Require(p.den > 0, "Rational denominator must be positive");
    for (const auto endpoints : {std::pair{a, b}, std::pair{c, d}}) {
      const auto [s, e] = endpoints;
      // Verify both original line equations using exact arithmetic.
      Require((p.x_num - Wide(s.x) * p.den) * (e.y - s.y) ==
                  (p.y_num - Wide(s.y) * p.den) * (e.x - s.x),
              "Returned point does not satisfy the original line");
    }
    const auto reversed = IntersectLines(b, a, d, c);
    Require(p.x_num == reversed.x_num && p.y_num == reversed.y_num &&
                p.den == reversed.den,
            "Reversing both line directions changed the intersection");
  }
}

void TestDistances() {
  Near(*DistanceToLine({3, 4}, {0, 0}, {5, 0}), 4);
  Near(*DistanceToLine({3, 4}, {0, 0}, {0, -2}), 3);
  Near(*DistanceToLine({2, 0}, {0, 0}, {1, 1}), std::sqrt(2.0L));
  Require(!DistanceToLine({3, 4}, {1, 1}, {1, 1}),
          "A point does not define an infinite line");
  Near(DistanceToSegment({2, 3}, {0, 0}, {4, 0}), 3);
  Near(DistanceToSegment({-3, 4}, {0, 0}, {4, 0}), 5);
  Near(DistanceToSegment({7, 4}, {0, 0}, {4, 0}), 5);
  Near(DistanceToSegment({7, 4}, {4, 0}, {0, 0}), 5);
  Near(DistanceToSegment({3, 4}, {0, 0}, {0, 0}), 5);
  Near(DistanceToSegment({0, 0}, {0, 0}, {0, 0}), 0);
  Near(DistanceToSegment({1000000000, 1000000000},
                         {-1000000000, -1000000000},
                         {1000000000, -1000000000}), 2000000000.0L);
}

}  // namespace

int main() {
  try {
    TestExhaustiveSegments();
    TestLineCases();
    TestRandomLines();
    TestDistances();
  } catch (const std::exception& error) {
    std::cerr << "Geometry segments test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Geometry segments tests passed\n";
}
