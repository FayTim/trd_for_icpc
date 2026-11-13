#pragma once

#include "../number_theory/factorization.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <vector>

namespace combinatorics {

// Exact C(n,k); k > n gives 0, overflow of uint64_t gives nullopt.
inline std::optional<std::uint64_t> Binomial(std::uint64_t n,
                                           std::uint64_t k) {
  if (k > n) return 0;
  k = std::min(k, n - k);
  std::uint64_t value = 1;
  for (std::uint64_t i = 1; i <= k; ++i) {
    const __uint128_t next =
        static_cast<__uint128_t>(value) * (n - k + i) / i;
    if (next > std::numeric_limits<std::uint64_t>::max())
      return std::nullopt;
    value = static_cast<std::uint64_t>(next);
  }
  return value;
}

// Preconditions: 0 <= max_n < prime; prime is prime. O(max_n) memory.
class PrimeBinomial {
 public:
  PrimeBinomial(int max_n, std::uint64_t prime)
      : mod_(prime), fact_(static_cast<std::size_t>(max_n) + 1),
        inv_fact_(fact_.size()) {
    fact_[0] = 1;
    for (std::size_t i = 1; i < fact_.size(); ++i)
      fact_[i] = nt::MulMod(fact_[i - 1], i, mod_);
    inv_fact_[max_n] = nt::PowMod(fact_[max_n], mod_ - 2, mod_);
    for (std::size_t i = fact_.size() - 1; i > 0; --i)
      inv_fact_[i - 1] = nt::MulMod(inv_fact_[i], i, mod_);
  }

  // 0 <= n <= max_n; k outside [0,n] gives 0.
  std::uint64_t Choose(int n, int k) const {
    if (k < 0 || k > n) return 0;
    return nt::MulMod(fact_[n],
                      nt::MulMod(inv_fact_[k], inv_fact_[n - k], mod_),
                      mod_);
  }

 private:
  std::uint64_t mod_;
  std::vector<std::uint64_t> fact_, inv_fact_;
};

}  // namespace combinatorics
