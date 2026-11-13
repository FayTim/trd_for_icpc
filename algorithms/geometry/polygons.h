#pragma once

#include <algorithm>
#include <vector>

#include "segments.h"

namespace geometry {

inline Wide SignedArea2(const std::vector<Point>& polygon) {
  Wide sum = 0;
  for (std::size_t i = 0; i < polygon.size(); ++i) {
    sum += Cross(polygon[i], polygon[(i + 1) % polygon.size()]);
  }
  return sum;
}

inline Wide Area2(const std::vector<Point>& polygon) {
  const Wide sum = SignedArea2(polygon);
  return sum < 0 ? -sum : sum;
}

enum class Location { Outside, Boundary, Inside };

inline Location LocatePoint(const std::vector<Point>& polygon, Point p) {
  int winding = 0;
  for (std::size_t i = 0; i < polygon.size(); ++i) {
    const Point a = polygon[i], b = polygon[(i + 1) % polygon.size()];
    if (OnSegment(p, a, b)) return Location::Boundary;
    if (a.y <= p.y && p.y < b.y && Orient(a, b, p) > 0) ++winding;
    if (b.y <= p.y && p.y < a.y && Orient(a, b, p) < 0) --winding;
  }
  return winding == 0 ? Location::Outside : Location::Inside;
}

inline std::vector<Point> ConvexHull(std::vector<Point> points) {
  std::sort(points.begin(), points.end());
  points.erase(std::unique(points.begin(), points.end()), points.end());
  if (points.size() <= 1) return points;

  std::vector<Point> lower, upper;
  for (Point p : points) {
    while (lower.size() >= 2 &&
           Orient(lower[lower.size() - 2], lower.back(), p) <= 0) {
      lower.pop_back();
    }
    lower.push_back(p);
  }
  for (auto it = points.rbegin(); it != points.rend(); ++it) {
    while (upper.size() >= 2 &&
           Orient(upper[upper.size() - 2], upper.back(), *it) <= 0) {
      upper.pop_back();
    }
    upper.push_back(*it);
  }
  lower.pop_back();
  upper.pop_back();
  lower.insert(lower.end(), upper.begin(), upper.end());
  return lower;
}

}  // namespace geometry
