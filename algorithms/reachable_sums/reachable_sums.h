#pragma once

#include <bitset>
#include <cstddef>
#include <vector>

template <std::size_t MaxSum>
std::bitset<MaxSum + 1> ReachableSums(const std::vector<int>& weights) {
  std::bitset<MaxSum + 1> reachable(1);
  for (int w : weights) {
    reachable |= reachable << w;
  }
  return reachable;
}
