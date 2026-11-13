#pragma once

#include <numeric>
#include <optional>

namespace answer_search {

// Inclusive [lo, hi]; predicate is false...false,true...true.
template <class Predicate>
std::optional<long long> FirstTrue(long long lo, long long hi,
                                  Predicate predicate) {
  if (lo > hi || !predicate(hi)) return std::nullopt;
  while (lo < hi) {
    const long long mid = std::midpoint(lo, hi);  // rounds toward lo
    if (predicate(mid)) hi = mid;
    else lo = mid + 1;  // mid < hi, so this cannot overflow
  }
  return lo;
}

// Inclusive [lo, hi]; predicate is true...true,false...false.
template <class Predicate>
std::optional<long long> LastTrue(long long lo, long long hi,
                                 Predicate predicate) {
  if (lo > hi || !predicate(lo)) return std::nullopt;
  while (lo < hi) {
    const long long mid = std::midpoint(hi, lo);  // rounds toward hi
    if (predicate(mid)) lo = mid;
    else hi = mid - 1;  // mid > lo, so this cannot overflow
  }
  return lo;
}

}  // namespace answer_search
