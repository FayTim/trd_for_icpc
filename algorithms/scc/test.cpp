#include "scc.h"

#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

namespace {

using Graph = std::vector<std::vector<int>>;

void Require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void CheckShapeAndOrder(const Graph& graph, const SccResult& result) {
  const int vertices = static_cast<int>(graph.size());
  Require(result.count >= 0 && result.count <= vertices,
          "Component count is outside the valid range");
  Require(result.component.size() == graph.size(),
          "Each vertex must have a component label");
  std::vector<bool> seen(result.count, false);
  for (int vertex = 0; vertex < vertices; ++vertex) {
    const int component = result.component[vertex];
    Require(component >= 0 && component < result.count,
            "Component label is outside the valid range");
    seen[component] = true;
    for (int to : graph[vertex]) {
      if (component != result.component[to]) {
        Require(component < result.component[to],
                "Component labels must follow topological order");
      }
    }
  }
  for (bool used : seen) {
    Require(used, "Component labels must be contiguous");
  }
}

void CheckAgainstReachability(const Graph& graph, int expected_count = -1) {
  const Graph original = graph;
  const SccResult result = FindScc(graph);
  Require(graph == original, "FindScc changed its input");
  CheckShapeAndOrder(graph, result);
  if (expected_count != -1) {
    Require(result.count == expected_count, "Unexpected number of components");
  }

  const std::size_t vertices = graph.size();
  std::vector<std::vector<bool>> reachable(
      vertices, std::vector<bool>(vertices, false));
  for (std::size_t vertex = 0; vertex < vertices; ++vertex) {
    reachable[vertex][vertex] = true;
    for (int to : graph[vertex]) {
      reachable[vertex][to] = true;
    }
  }
  for (std::size_t via = 0; via < vertices; ++via) {
    for (std::size_t from = 0; from < vertices; ++from) {
      for (std::size_t to = 0; to < vertices; ++to) {
        reachable[from][to] = reachable[from][to] ||
                             (reachable[from][via] && reachable[via][to]);
      }
    }
  }
  for (std::size_t from = 0; from < vertices; ++from) {
    for (std::size_t to = 0; to < vertices; ++to) {
      const bool same_component = result.component[from] == result.component[to];
      Require(same_component == (reachable[from][to] && reachable[to][from]),
              "SCC partition disagrees with mutual reachability");
    }
  }
}

void TestKnownGraphs() {
  CheckAgainstReachability({}, 0);
  CheckAgainstReachability({{}}, 1);
  CheckAgainstReachability(Graph(5), 5);
  CheckAgainstReachability({{0, 0}, {1}, {2, 2}}, 3);
  CheckAgainstReachability({{1, 1}, {0, 2, 2}, {3}, {2}, {}}, 3);
  CheckAgainstReachability({{1}, {2}, {3}, {0}}, 1);
  CheckAgainstReachability({{1, 2}, {3}, {3}, {}, {5}, {}}, 6);
  CheckAgainstReachability({{1}, {0, 2}, {3}, {4}, {3}}, 3);
}

void TestRandomizedAgainstReachability() {
  std::mt19937 generator(0x5CC);
  std::uniform_int_distribution<int> size_distribution(0, 9);
  std::uniform_int_distribution<int> edge_distribution(0, 5);
  for (int trial = 0; trial < 500; ++trial) {
    const int vertices = size_distribution(generator);
    Graph graph(vertices);
    for (int from = 0; from < vertices; ++from) {
      for (int to = 0; to < vertices; ++to) {
        const int draw = edge_distribution(generator);
        if (draw <= trial % 4) {
          graph[from].push_back(to);
          if (draw == 0) {
            graph[from].push_back(to);
          }
        }
      }
    }
    CheckAgainstReachability(graph);
  }
}

void TestDeepGraphs() {
  constexpr int kVertices = 100000;
  Graph graph(kVertices);
  for (int vertex = 0; vertex + 1 < kVertices; ++vertex) {
    graph[vertex].push_back(vertex + 1);
  }
  const SccResult chain = FindScc(graph);
  CheckShapeAndOrder(graph, chain);
  Require(chain.count == kVertices, "Each chain vertex must form its own SCC");

  graph.back().push_back(0);
  const SccResult cycle = FindScc(graph);
  CheckShapeAndOrder(graph, cycle);
  Require(cycle.count == 1, "A directed cycle must form one SCC");
}

}  // namespace

int main() {
  try {
    TestKnownGraphs();
    TestRandomizedAgainstReachability();
    TestDeepGraphs();
  } catch (const std::exception& error) {
    std::cerr << "SCC test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "SCC tests passed\n";
}
