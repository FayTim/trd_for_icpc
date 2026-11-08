#include "scc.h"

#include <iostream>
#include <vector>

int main() {
  // Components form a chain: {0, 1} -> {2} -> {3, 4}.
  const std::vector<std::vector<int>> graph{{1}, {0, 2}, {3}, {4}, {3}};
  const SccResult result = FindScc(graph);

  std::cout << result.count << '\n';  // 3
  for (std::size_t vertex = 0; vertex < graph.size(); ++vertex) {
    if (vertex != 0) {
      std::cout << ' ';
    }
    std::cout << result.component[vertex];
  }
  std::cout << '\n';  // 0 0 1 2 2
}
