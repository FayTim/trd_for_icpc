#pragma once

#include <cstdint>
#include <vector>

namespace nt {

// Index 0 is ignored. All intermediate sums must fit int64_t.
inline std::vector<std::int64_t> SumOverMultiples(
    const std::vector<std::int64_t>& values) {
  std::vector<std::int64_t> sums(values.size());
  for (std::size_t d = 1; d < values.size(); ++d)
    for (std::size_t x = d; x < values.size(); x += d)
      sums[d] += values[x];
  return sums;
}

inline std::vector<std::int64_t> InvertMultiples(
    std::vector<std::int64_t> sums) {
  for (std::size_t d = sums.size(); d-- > 1;)
    for (std::size_t x = 2 * d; x < sums.size(); x += d)
      sums[d] -= sums[x];
  if (!sums.empty()) sums[0] = 0;
  return sums;
}

inline std::vector<std::int64_t> SumOverDivisors(
    const std::vector<std::int64_t>& values) {
  std::vector<std::int64_t> sums(values.size());
  for (std::size_t d = 1; d < values.size(); ++d)
    for (std::size_t x = d; x < values.size(); x += d)
      sums[x] += values[d];
  return sums;
}

inline std::vector<std::int64_t> InvertDivisors(
    std::vector<std::int64_t> sums) {
  for (std::size_t d = 1; d < sums.size(); ++d)
    for (std::size_t x = 2 * d; x < sums.size(); x += d)
      sums[x] -= sums[d];
  if (!sums.empty()) sums[0] = 0;
  return sums;
}

// Nonnegative frequencies of positive values; total <= 2^32.
inline std::vector<std::int64_t> ExactGcdPairs(
    const std::vector<std::int64_t>& frequency) {
  auto pairs = SumOverMultiples(frequency);
  for (auto& count : pairs)
    count = count % 2 ? count * ((count - 1) / 2)
                      : (count / 2) * (count - 1);
  return InvertMultiples(pairs);
}

// n >= 1, p prime. For n! the input n may also be zero.
inline std::uint64_t Valuation(std::uint64_t n, std::uint64_t p) {
  std::uint64_t exponent = 0;
  while (n % p == 0) { n /= p; ++exponent; }
  return exponent;
}

inline std::uint64_t FactorialValuation(std::uint64_t n, std::uint64_t p) {
  std::uint64_t exponent = 0;
  while (n >= p) { n /= p; exponent += n; }
  return exponent;
}

// visit(l, r, q): floor(n / i) == q for every i in [l, r].
template <class F>
void QuotientBlocks(std::uint64_t n, F visit) {
  for (std::uint64_t l = 1; l <= n;) {
    const std::uint64_t q = n / l, r = n / q;
    visit(l, r, q);
    if (r == n) break;
    l = r + 1;
  }
}

}  // namespace nt
