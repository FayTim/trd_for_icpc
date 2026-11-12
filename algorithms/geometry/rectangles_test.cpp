#include "rectangles.h"

#include <array>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

namespace {

using namespace geometry;

void Require(bool condition, const char* message) {
  if (!condition) throw std::runtime_error(message);
}

bool Same(const std::optional<Rect>& a, const std::optional<Rect>& b) {
  return a.has_value() == b.has_value() &&
         (!a || (a->low == b->low && a->high == b->high));
}

// Enumerate half-integer points. The domain has integral boundaries,
// so every nonempty intersection has an enumerated point.
using Grid = std::array<bool, 81>;

Grid Enumerate(Rect r) {
  Grid points{};
  for (long long x = 2 * r.low.x; x <= 2 * r.high.x; ++x) {
    for (long long y = 2 * r.low.y; y <= 2 * r.high.y; ++y) {
      points[(x + 4) * 9 + (y + 4)] = true;
    }
  }
  return points;
}

void TestExhaustiveIntersections() {
  std::vector<Rect> rectangles;
  std::vector<Grid> grids;
  for (int lx = -2; lx <= 2; ++lx) {
    for (int hx = lx; hx <= 2; ++hx) {
      for (int ly = -2; ly <= 2; ++ly) {
        for (int hy = ly; hy <= 2; ++hy) {
          const Rect r = MakeRect({hx, hy}, {lx, ly});
          Require(r.low == Point{lx, ly} && r.high == Point{hx, hy},
                  "MakeRect did not normalize opposite corners");
          rectangles.push_back(r);
          grids.push_back(Enumerate(r));
          const Rect crossed = MakeRect({lx, hy}, {hx, ly});
          Require(crossed.low == r.low && crossed.high == r.high,
                  "MakeRect did not normalize mixed corner order");
        }
      }
    }
  }
  for (std::size_t i = 0; i < rectangles.size(); ++i) {
    const Rect a = rectangles[i];
    for (int x = -2; x <= 2; ++x) {
      for (int y = -2; y <= 2; ++y) {
        Require(Contains(a, {x, y}) == grids[i][(2*x+4)*9+2*y+4],
                "Contains disagrees with enumerated lattice points");
      }
    }
    for (std::size_t j = 0; j < rectangles.size(); ++j) {
      const auto intersection = Intersect(a, rectangles[j]);
      Require(Same(intersection, Intersect(rectangles[j], a)),
              "Intersection is not commutative");
      bool nonempty = false;
      int unit_cells = 0;
      for (int x = -4; x <= 4; ++x) {
        for (int y = -4; y <= 4; ++y) {
          const int index = (x + 4) * 9 + (y + 4);
          const bool expected = grids[i][index] && grids[j][index];
          nonempty |= expected;
          if (expected && x % 2 != 0 && y % 2 != 0) ++unit_cells;
          const bool actual = intersection &&
              2 * intersection->low.x <= x &&
              x <= 2 * intersection->high.x &&
              2 * intersection->low.y <= y &&
              y <= 2 * intersection->high.y;
          Require(actual == expected,
                  "Intersection disagrees with enumerated points");
        }
      }
      Require(intersection.has_value() == nonempty,
              "Empty or touching intersection classified incorrectly");
      if (intersection) {
        Require(RectangleArea(*intersection) == unit_cells,
                "Intersection area disagrees with counted unit cells");
      }
    }
  }
}

void TestBoundingBox() {
  Require(!BoundingBox({}), "Empty point set has no bounding box");
  const auto one = BoundingBox({{3, -4}});
  Require(one && one->low == Point{3, -4} && one->high == one->low,
          "Singleton bounding box must be a point");
  std::mt19937 random(1234567);
  for (int trial = 0; trial < 500; ++trial) {
    std::vector<Point> points;
    const int count = 1 + random() % 40;
    for (int i = 0; i < count; ++i) {
      points.push_back({static_cast<int>(random() % 41) - 20,
                        static_cast<int>(random() % 41) - 20});
    }
    const auto box = BoundingBox(points);
    bool left = false, right = false, bottom = false, top = false;
    for (Point p : points) {
      Require(Contains(*box, p), "Bounding box omitted an input point");
      left |= p.x == box->low.x;
      right |= p.x == box->high.x;
      bottom |= p.y == box->low.y;
      top |= p.y == box->high.y;
    }
    Require(left && right && bottom && top,
            "Bounding box is not minimal on every side");
    std::shuffle(points.begin(), points.end(), random);
    Require(Same(box, BoundingBox(points)),
            "Bounding box changed after point permutation");
  }
}

void TestLargeAndDegenerate() {
  constexpr long long k = 1000000000;
  const Rect whole = MakeRect({-k, -k}, {k, k});
  Require(RectangleArea(whole) == Wide(4) * k * k,
          "Large rectangle area overflowed");
  Require(Perimeter(whole) == 8 * k, "Large perimeter overflowed");
  const auto line = Intersect(whole, MakeRect({k, -k}, {k, k}));
  Require(line && RectangleArea(*line) == 0 && Perimeter(*line) == 4*k,
          "Boundary line must be a nonempty zero-area rectangle");
  const Rect point = MakeRect({-k, k}, {-k, k});
  Require(RectangleArea(point) == 0 && Perimeter(point) == 0,
          "Point rectangle must have zero area and perimeter");
  Require(Contains(point, {-k, k}) && !Contains(point, {-k + 1, k}),
          "Point rectangle containment is incorrect");
  Require(!Intersect(MakeRect({0, 0}, {1, 1}),
                     MakeRect({2, 0}, {3, 1})),
          "Separated rectangles should not intersect");
}

}  // namespace

int main() {
  try {
    TestExhaustiveIntersections();
    TestBoundingBox();
    TestLargeAndDegenerate();
  } catch (const std::exception& error) {
    std::cerr << "Rectangle test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "Rectangle tests passed\n";
}
