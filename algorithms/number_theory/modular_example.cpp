#include "modular.h"
#include <iostream>

int main() {
  const auto equation = nt::SolveCongruence(6, 8, 14);
  std::cout << equation->first << ' ' << equation->second << '\n'; // 6 7
  const auto merged = nt::MergeCongruences(2, 6, 5, 9);
  std::cout << merged->first << ' ' << merged->second << '\n'; // 14 18
  std::cout << *nt::ModInverse(-3, 11) << '\n'; // 7
}
