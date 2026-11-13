#include "euler.h"

#include <iostream>
#include <utility>
#include <vector>

int main() {
  // Directed edges: ids 0, 1, 2; the only possible start is 0.
  const std::vector<std::pair<int, int>> edges{{0, 1}, {1, 0}, {0, 2}};
  auto walk = euler_h::eulerWalk(3, edges, true);
  if (!walk) {
    std::cout << "No Euler trail\n";
    return 0;
  }
  for (int v : walk->vertices) std::cout << v << ' ';  // 0 1 0 2
  std::cout << '\n';
  for (int id : walk->edge_ids) std::cout << id << ' ';  // 0 1 2
  std::cout << '\n';

  // Undirected parallel edges and a loop. Insert each edge only once.
  auto cycle = euler_h::eulerWalk(2, {{0, 1}, {0, 1}, {1, 1}}, false);
  std::cout << (cycle && cycle->vertices.front() == cycle->vertices.back())
            << '\n';  // 1
}
