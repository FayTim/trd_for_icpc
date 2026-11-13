#include "gray_code.h"

#include <array>
#include <bit>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

bool Check(bool ok, const char* message) {
  if (!ok) std::cerr << message << '\n';
  return ok;
}

int main() {
  using gray_code::ChangedBit;
  using gray_code::Decode;
  using gray_code::Encode;
  constexpr std::array<std::uint64_t, 8> expected{0, 1, 3, 2, 6, 7, 5, 4};
  for (std::uint64_t i = 0; i < expected.size(); ++i) {
    if (!Check(Encode(i) == expected[i], "Known sequence differs")) return 1;
  }

  for (int bits = 0; bits <= 16; ++bits) {
    const std::uint64_t count = std::uint64_t{1} << bits;
    std::vector<bool> seen(count, false);
    for (std::uint64_t i = 0; i < count; ++i) {
      const auto gray = Encode(i);
      if (!Check(gray < count && !seen[gray],
                 "Traversal must visit each mask once") ||
          !Check(Decode(gray) == i, "Decode failed")) return 1;
      seen[gray] = true;
      if (i != 0) {
        const auto difference = gray ^ Encode(i - 1);
        if (!Check(std::popcount(difference) == 1,
                   "Consecutive masks must differ in one bit") ||
            !Check(difference == (std::uint64_t{1} << ChangedBit(i)),
                   "ChangedBit reports the wrong bit")) return 1;
      }
    }
    if (bits != 0 &&
        !Check(std::popcount(Encode(count - 1) ^ Encode(0)) == 1,
               "The traversal must close into a cycle")) return 1;
  }

  for (int bit = 0; bit < 64; ++bit) {
    const std::uint64_t index = std::uint64_t{1} << bit;
    if (!Check(ChangedBit(index) == bit &&
               (Encode(index) ^ Encode(index - 1)) == index,
               "ChangedBit failed at a power of two") ||
        !Check(Decode(Encode(index)) == index,
               "Decode failed at a high bit")) return 1;
  }
  constexpr auto maximum = std::numeric_limits<std::uint64_t>::max();
  for (auto x : {maximum, maximum - 1, maximum / 2, std::uint64_t{0}}) {
    if (!Check(Decode(Encode(x)) == x && Encode(Decode(x)) == x,
               "Full-width round trip failed")) return 1;
  }
  std::uint64_t state = 0x7c0913a45bcdef10ULL;
  for (int i = 0; i < 10000; ++i) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    if (!Check(Decode(Encode(state)) == state &&
               Encode(Decode(state)) == state,
               "Random full-width round trip failed")) return 1;
  }
}
