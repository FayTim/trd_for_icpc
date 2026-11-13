#pragma once

#include <algorithm>
#include <cstddef>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

inline auto CompressCoordinates(const std::vector<int>& coords) {
  auto from = coords;
  std::sort(from.begin(), from.end());
  from.erase(std::unique(from.begin(), from.end()), from.end());

  std::unordered_map<int, std::size_t> to;
  for (std::size_t i = 0; i < from.size(); ++i) {
    to[from[i]] = i;
  }

  std::vector<std::size_t> compressed;
  compressed.reserve(coords.size());
  for (int x : coords) {
    compressed.push_back(to.at(x));
  }

  return std::tuple{
      std::move(to), std::move(from), std::move(compressed)};
}
