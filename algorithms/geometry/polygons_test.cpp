#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "polygons.h"

using geometry::Area2;
using geometry::ConvexHull;
using geometry::Distance2;
using geometry::LocatePoint;
using geometry::Location;
using geometry::Orient;
using geometry::Point;
using geometry::SignedArea2;
using geometry::Wide;

void Require(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "polygons: " << message << '\n';
    std::exit(1);
  }
}

// Independent floating-point ray crossing; test coordinates are small.
Location RayReference(const std::vector<Point>& polygon, Point p) {
  bool inside = false;
  for (std::size_t i = 0; i < polygon.size(); ++i) {
    const Point a = polygon[i], b = polygon[(i + 1) % polygon.size()];
    const long double dx = b.x - a.x, dy = b.y - a.y;
    const long double cross = dx * (p.y - a.y) - dy * (p.x - a.x);
    if (cross == 0 && p.x >= std::min(a.x, b.x) &&
        p.x <= std::max(a.x, b.x) && p.y >= std::min(a.y, b.y) &&
        p.y <= std::max(a.y, b.y)) {
      return Location::Boundary;
    }
    if ((a.y > p.y) != (b.y > p.y)) {
      const long double x = a.x + dx * (p.y - a.y) / dy;
      if (p.x < x) inside = !inside;
    }
  }
  return inside ? Location::Inside : Location::Outside;
}

// Jarvis march is structurally independent from the monotone chain.
std::vector<Point> GiftWrap(std::vector<Point> points) {
  std::sort(points.begin(), points.end());
  points.erase(std::unique(points.begin(), points.end()), points.end());
  if (points.size() <= 1) return points;
  std::vector<Point> hull;
  Point current = points.front();
  do {
    hull.push_back(current);
    Point next = points[0] == current ? points[1] : points[0];
    for (Point candidate : points) {
      if (candidate == current) continue;
      const Wide turn = Orient(current, next, candidate);
      if (turn < 0 || (turn == 0 &&
          Distance2(current, candidate) > Distance2(current, next))) {
        next = candidate;
      }
    }
    current = next;
    Require(hull.size() <= points.size(), "reference hull terminates");
  } while (current != points.front());
  return hull;
}

void TestFixed() {
  Require(Area2({}) == 0, "empty area");
  Require(LocatePoint({}, {0, 0}) == Location::Outside, "empty location");
  Require(LocatePoint({{2, 3}}, {2, 3}) == Location::Boundary, "one point");
  Require(LocatePoint({{2, 3}}, {3, 3}) == Location::Outside, "one outside");
  const std::vector<Point> segment{{0, 0}, {4, 0}};
  Require(Area2(segment) == 0, "segment area");
  Require(LocatePoint(segment, {2, 0}) == Location::Boundary, "segment");
  Require(LocatePoint(segment, {5, 0}) == Location::Outside, "extension");
  Require(LocatePoint(segment, {2, 1}) == Location::Outside, "segment side");
  Require(ConvexHull({}).empty(), "empty hull");
  Require(ConvexHull({{2, 3}, {2, 3}}) ==
          std::vector<Point>{{2, 3}}, "duplicate singleton");
  Require(ConvexHull({{0, 0}, {2, 2}, {1, 1}, {-1, -1}}) ==
          std::vector<Point>{{-1, -1}, {2, 2}}, "collinear hull");

  const std::vector<Point> concave{
      {0, 0}, {6, 0}, {6, 2}, {2, 2}, {2, 6}, {0, 6}};
  Require(SignedArea2(concave) == 40, "concave area");
  Require(LocatePoint(concave, {1, 4}) == Location::Inside, "concave arm");
  Require(LocatePoint(concave, {4, 4}) == Location::Outside, "concave notch");
  Require(LocatePoint(concave, {2, 4}) == Location::Boundary, "concave edge");
  Require(LocatePoint(concave, {2, 2}) == Location::Boundary, "reflex vertex");
  const std::vector<Point> box{
      {-1000000000, -1000000000}, {1000000000, -1000000000},
      {1000000000, 1000000000}, {-1000000000, 1000000000}};
  Require(Area2(box) == Wide(8) * 1000000000 * 1000000000, "large area");
  Require(LocatePoint(box, {0, 0}) == Location::Inside, "large location");
  Require(ConvexHull(box) == box, "large hull");
}

void TestRandom() {
  std::mt19937 rng(1729);
  for (int trial = 0; trial < 1500; ++trial) {
    std::vector<Point> points;
    const int n = static_cast<int>(rng() % 25);
    for (int i = 0; i < n; ++i) {
      points.push_back({static_cast<int>(rng() % 17) - 8,
                        static_cast<int>(rng() % 17) - 8});
    }
    const auto hull = ConvexHull(points);
    Require(hull == GiftWrap(points), "random hull vs Jarvis march");
    for (Point p : points) {
      Require(LocatePoint(hull, p) != Location::Outside, "hull contains input");
    }
    if (hull.size() >= 3) {
      Require(SignedArea2(hull) > 0, "hull counterclockwise");
      for (std::size_t i = 0; i < hull.size(); ++i) {
        Require(Orient(hull[i], hull[(i + 1) % hull.size()],
                       hull[(i + 2) % hull.size()]) > 0,
                "hull has strictly convex corners");
      }
    }
    std::shuffle(points.begin(), points.end(), rng);
    Require(ConvexHull(points) == hull, "hull input order independence");
  }

  // Increasing polar angles give simple, often concave, radial polygons.
  const std::vector<Point> rays{
      {1, 0}, {1, 1}, {0, 1}, {-1, 1},
      {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
  for (int trial = 0; trial < 150; ++trial) {
    std::vector<Point> polygon;
    for (Point v : rays) {
      const int radius = 1 + static_cast<int>(rng() % 8);
      polygon.push_back({v.x * radius, v.y * radius});
    }
    auto reverse = polygon;
    std::reverse(reverse.begin(), reverse.end());
    auto shifted = polygon;
    for (Point& v : shifted) v = v + Point{37, -51};
    Require(SignedArea2(reverse) == -SignedArea2(polygon), "area reversal");
    Require(Area2(shifted) == Area2(polygon), "area translation");
    for (int x = -9; x <= 9; ++x) {
      for (int y = -9; y <= 9; ++y) {
        const Point p{x, y};
        const Location expected = RayReference(polygon, p);
        Require(LocatePoint(polygon, p) == expected, "ray crossing reference");
        Require(LocatePoint(reverse, p) == expected, "location reversal");
        Require(LocatePoint(shifted, p + Point{37, -51}) == expected,
                "location translation");
      }
    }
  }
}

int main() {
  TestFixed();
  TestRandom();
  std::cout << "polygon tests passed\n";
}
