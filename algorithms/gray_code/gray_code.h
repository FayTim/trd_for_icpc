#pragma once

#include <bit>
#include <cstdint>

namespace gray_code {

inline std::uint64_t Encode(std::uint64_t index) {
  return index ^ (index >> 1);
}

inline std::uint64_t Decode(std::uint64_t gray) {
  std::uint64_t index = 0;
  for (; gray != 0; gray >>= 1) index ^= gray;
  return index;
}

// index > 0; bit changed from Encode(index - 1) to Encode(index).
inline int ChangedBit(std::uint64_t index) {
  return std::countr_zero(index);
}

}  // namespace gray_code
