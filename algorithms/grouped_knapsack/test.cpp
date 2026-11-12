#include "grouped_knapsack.h"

#include <stdexcept>

void Check(const std::vector<std::vector<KnapsackOption>>& groups, int capacity,
           std::optional<long long> expected, const char* message) {
  if (GroupedKnapsack(groups, capacity) != expected) {
    throw std::runtime_error(message);
  }
}

int main() {
  Check({}, 0, 0, "No groups");
  Check({{{1, 6}, {2, 8}}, {{1, 3}, {3, 4}}}, 4, 11,
        "Exactly one option per group with spare capacity");
  Check({{{0, 5}, {0, 7}}, {{0, -4}}}, 0, 3,
        "Zero weights and a mandatory negative value");
  Check({{{1, -3}}, {{0, -2}}}, 1, -5, "Negative optimum");
  Check({{{0, 0}, {1, -1}}}, 1, 0, "Optional group via a zero option");
  Check({{{0, 9}}, {}, {{0, 4}}}, 0, std::nullopt, "Empty group");
  Check({{{3, 9}}, {{2, 4}}}, 4, std::nullopt,
        "Groups fit separately but not together");
  Check({{{10, 9}, {2, 4}}}, 2, 4, "Option exceeds capacity");
}
