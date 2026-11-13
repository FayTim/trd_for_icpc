#include "assignment.h"

#include <numeric>
#include <stdexcept>

void CheckAgainstPermutations(const std::vector<std::vector<int>>& cost) {
  std::vector<std::size_t> seats(cost.size());
  std::iota(seats.begin(), seats.end(), std::size_t{0});
  long long best = std::numeric_limits<long long>::max();
  do {
    long long total = 0;
    for (std::size_t person = 0; person < cost.size(); ++person) {
      total += cost[person][seats[person]];
    }
    best = std::min(best, total);
  } while (std::next_permutation(seats.begin(), seats.end()));

  if (AssignmentCost(cost) != best) {
    throw std::runtime_error("Assignment optimum differs from exhaustive search");
  }
}

int main() {
  CheckAgainstPermutations({});
  CheckAgainstPermutations({{-7}});
  CheckAgainstPermutations({{0, 2}, {1, 10}});
  CheckAgainstPermutations({{6, 1, 5}, {2, 4, 7}, {3, 6, 2}});
  CheckAgainstPermutations({{-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}});
  CheckAgainstPermutations({{1500000000, 1600000000}, {1600000000, 1500000000}});
}
