#include "gray_code.h"

#include <cstdint>
#include <iostream>

int main() {
  const int bits = 3;  // 0 <= bits < 64 for this loop bound
  const std::uint64_t count = std::uint64_t{1} << bits;
  std::uint64_t mask = 0;
  std::cout << mask << '\n';
  for (std::uint64_t i = 1; i < count; ++i) {
    const int bit = gray_code::ChangedBit(i);
    mask ^= std::uint64_t{1} << bit;
    // Update only the contribution of bit; mask == gray_code::Encode(i).
    std::cout << mask << " (changed bit " << bit << ")\n";
  }
}
