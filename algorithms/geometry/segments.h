#pragma once

#include "point.h"

#include <cmath>
#include <limits>
#include <optional>

namespace geometry {

inline bool OnSegment(Point p, Point a, Point b) {
  return Orient(a, b, p) == 0 &&
         std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x) &&
         std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y);
}

inline bool SegmentsIntersect(Point a, Point b, Point c, Point d) {
  if (OnSegment(a, c, d) || OnSegment(b, c, d) ||
      OnSegment(c, a, b) || OnSegment(d, a, b)) return true;
  return Sign(Orient(a, b, c)) * Sign(Orient(a, b, d)) < 0 &&
         Sign(Orient(c, d, a)) * Sign(Orient(c, d, b)) < 0;
}

enum class LineKind { Unique, Parallel, Coincident, Degenerate };

struct LineIntersection {
  LineKind kind;
  Wide x_num = 0, y_num = 0, den = 1;
};

inline LineIntersection IntersectLines(
    Point a, Point b, Point c, Point d) {
  if (a == b || c == d) return {LineKind::Degenerate};
  const Point u = b - a, v = d - c;
  Wide den = Cross(u, v);
  if (den == 0) {
    return {Orient(a, b, c) == 0 ? LineKind::Coincident
                                : LineKind::Parallel};
  }
  const Wide t = Cross(c - a, v);
  Wide x = Wide(a.x) * den + Wide(u.x) * t;
  Wide y = Wide(a.y) * den + Wide(u.y) * t;
  if (den < 0) { den = -den; x = -x; y = -y; }
  return {LineKind::Unique, x, y, den};
}

inline std::optional<Point> IntegerPoint(const LineIntersection& p) {
  if (p.kind != LineKind::Unique || p.x_num % p.den != 0 ||
      p.y_num % p.den != 0) return std::nullopt;
  const Wide x = p.x_num / p.den, y = p.y_num / p.den;
  const Wide lo = std::numeric_limits<long long>::min();
  const Wide hi = std::numeric_limits<long long>::max();
  if (x < lo || x > hi || y < lo || y > hi) return std::nullopt;
  return Point{static_cast<long long>(x), static_cast<long long>(y)};
}

inline std::optional<long double> DistanceToLine(
    Point p, Point a, Point b) {
  if (a == b) return std::nullopt;
  return std::abs(static_cast<long double>(Orient(a, b, p))) /
         std::sqrt(static_cast<long double>(Distance2(a, b)));
}

inline long double DistanceToSegment(Point p, Point a, Point b) {
  const Wide length2 = Distance2(a, b);
  const Wide projection = Dot(p - a, b - a);
  if (projection <= 0)
    return std::sqrt(static_cast<long double>(Distance2(p, a)));
  if (projection >= length2)
    return std::sqrt(static_cast<long double>(Distance2(p, b)));
  return *DistanceToLine(p, a, b);
}

}  // namespace geometry
