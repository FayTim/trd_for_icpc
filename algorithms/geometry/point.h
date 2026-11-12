#pragma once

#include <algorithm>
#include <compare>
#include <cstdint>
#include <numeric>
#include <string>

namespace geometry {

using Wide = __int128_t;

struct Point {
  long long x = 0, y = 0;
  auto operator<=>(const Point&) const = default;
  Point operator+(Point p) const { return {x + p.x, y + p.y}; }
  Point operator-(Point p) const { return {x - p.x, y - p.y}; }
};

inline Wide Dot(Point a, Point b) {
  return Wide(a.x) * b.x + Wide(a.y) * b.y;
}

inline Wide Cross(Point a, Point b) {
  return Wide(a.x) * b.y - Wide(a.y) * b.x;
}

inline Wide Orient(Point a, Point b, Point c) {
  return Cross(b - a, c - a);
}

inline int Sign(Wide value) { return (value > 0) - (value < 0); }
inline Wide Distance2(Point a, Point b) { return Dot(b - a, b - a); }
inline Point Perp(Point v) { return {-v.y, v.x}; }

inline Point PrimitiveDirection(Point v) {
  const long long g = std::gcd(v.x, v.y);
  return g == 0 ? v : Point{v.x / g, v.y / g};
}

inline Point LineDirection(Point v) {
  v = PrimitiveDirection(v);
  return v.x < 0 || (v.x == 0 && v.y < 0) ? Point{-v.x, -v.y} : v;
}

inline std::string ToString(Wide value) {
  const bool negative = value < 0;
  __uint128_t magnitude = static_cast<__uint128_t>(value);
  if (negative) magnitude = 0 - magnitude;
  std::string result;
  do {
    result += char('0' + magnitude % 10);
    magnitude /= 10;
  } while (magnitude != 0);
  if (negative) result += '-';
  std::reverse(result.begin(), result.end());
  return result;
}

}  // namespace geometry
