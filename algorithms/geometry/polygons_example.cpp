#include <iostream>
#include <vector>

#include "polygons.h"

int main() {
  using namespace geometry;
  const std::vector<Point> points = {
      {0, 0}, {4, 0}, {4, 3}, {0, 3}, {2, 1}, {4, 0}};
  const auto hull = ConvexHull(points);
  std::cout << hull.size() << '\n';
  std::cout << ToString(Area2(hull)) << '\n';
  for (Point p : std::vector<Point>{{2, 1}, {4, 2}, {5, 2}}) {
    switch (LocatePoint(hull, p)) {
      case Location::Inside: std::cout << "inside\n"; break;
      case Location::Boundary: std::cout << "boundary\n"; break;
      case Location::Outside: std::cout << "outside\n"; break;
    }
  }
}
