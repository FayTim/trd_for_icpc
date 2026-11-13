#pragma once

#include "point.h"

#include <optional>
#include <vector>

namespace geometry {

struct Rect {
  Point low, high;
};

inline Rect MakeRect(Point a, Point b) {
  return {{std::min(a.x, b.x), std::min(a.y, b.y)},
          {std::max(a.x, b.x), std::max(a.y, b.y)}};
}

inline std::optional<Rect> BoundingBox(const std::vector<Point>& points) {
  if (points.empty()) return std::nullopt;
  Rect box{points[0], points[0]};
  for (Point p : points) {
    box.low.x = std::min(box.low.x, p.x);
    box.low.y = std::min(box.low.y, p.y);
    box.high.x = std::max(box.high.x, p.x);
    box.high.y = std::max(box.high.y, p.y);
  }
  return box;
}

inline std::optional<Rect> Intersect(Rect a, Rect b) {
  Rect r{{std::max(a.low.x, b.low.x), std::max(a.low.y, b.low.y)},
         {std::min(a.high.x, b.high.x),
          std::min(a.high.y, b.high.y)}};
  if (r.low.x > r.high.x || r.low.y > r.high.y) {
    return std::nullopt;
  }
  return r;
}

inline bool Contains(Rect r, Point p) {
  return r.low.x <= p.x && p.x <= r.high.x &&
         r.low.y <= p.y && p.y <= r.high.y;
}

inline Wide RectangleArea(Rect r) {
  return Wide(r.high.x - r.low.x) * (r.high.y - r.low.y);
}

inline long long Perimeter(Rect r) {
  return 2 * ((r.high.x - r.low.x) + (r.high.y - r.low.y));
}

}  // namespace geometry
