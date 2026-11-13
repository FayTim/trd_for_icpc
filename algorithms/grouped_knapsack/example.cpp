#include "grouped_knapsack.h"

#include <iostream>

int main() {
  const auto best = GroupedKnapsack(
      {{{1, 6}, {2, 8}}, {{1, 3}, {3, 4}}}, 4);
  if (best) std::cout << *best << '\n';  // 11
}
