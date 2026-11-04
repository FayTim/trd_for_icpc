#include "compress_coordinates.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void Require(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void CheckCompression(const std::vector<int>& input,
                      const std::vector<int>& expected_from,
                      const std::vector<std::size_t>& expected_compressed) {
  const std::vector<int> original = input;
  const auto [to, from, compressed] = CompressCoordinates(input);

  Require(input == original, "Input changed during compression");
  Require(from == expected_from, "Inverse mapping is not sorted and unique");
  Require(compressed == expected_compressed,
          "Compressed coordinates differ from expected ranks");
  Require(to.size() == from.size(), "Forward mapping has the wrong size");
  Require(compressed.size() == input.size(), "Input length was not preserved");

  for (std::size_t rank = 0; rank < from.size(); ++rank) {
    const auto entry = to.find(from[rank]);
    Require(entry != to.end(), "Forward mapping is missing a coordinate");
    Require(entry->second == rank, "Forward and inverse mappings disagree");
    if (rank != 0) {
      Require(from[rank - 1] < from[rank], "Coordinate order was not preserved");
    }
  }
  for (std::size_t index = 0; index < input.size(); ++index) {
    Require(compressed[index] < from.size(), "Compressed coordinate is out of range");
    Require(from[compressed[index]] == input[index],
            "Decompression does not reproduce the input");
    Require(to.at(input[index]) == compressed[index],
            "Compressed coordinate disagrees with the forward mapping");
  }
}

void TestExamplesAndBoundaries() {
  CheckCompression({}, {}, {});
  CheckCompression({42}, {42}, {0});
  CheckCompression({-7, -7, -7, -7}, {-7}, {0, 0, 0, 0});
  CheckCompression({-5, 0, 2, 100}, {-5, 0, 2, 100}, {0, 1, 2, 3});
  CheckCompression({100, 2, 0, -5}, {-5, 0, 2, 100}, {3, 2, 1, 0});
  CheckCompression({50, 10, 50, 20, 10}, {10, 20, 50}, {2, 0, 2, 1, 0});

  const int low = std::numeric_limits<int>::min();
  const int high = std::numeric_limits<int>::max();
  CheckCompression({40, -10, 40, 0, high, low},
                   {low, -10, 0, 40, high}, {3, 1, 3, 2, 4, 0});
  CheckCompression({high, low, high, low + 1, high - 1, low},
                   {low, low + 1, high - 1, high}, {3, 0, 3, 1, 2, 0});
}

void TestRandomizedAgainstReference() {
  std::mt19937 generator(0xC001D00D);
  std::uniform_int_distribution<int> full_range(
      std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
  std::uniform_int_distribution<int> small_range(-20, 20);

  for (int trial = 0; trial < 100; ++trial) {
    std::vector<int> input;
    for (int index = 0; index < trial * 13; ++index) {
      input.push_back(trial % 2 == 0 ? full_range(generator)
                                    : small_range(generator));
    }
    std::vector<int> reference = input;
    std::sort(reference.begin(), reference.end());
    reference.erase(std::unique(reference.begin(), reference.end()), reference.end());

    std::vector<std::size_t> expected;
    for (int value : input) {
      expected.push_back(static_cast<std::size_t>(
          std::lower_bound(reference.begin(), reference.end(), value) - reference.begin()));
    }
    CheckCompression(input, reference, expected);
  }
}

}  // namespace

int main() {
  try {
    TestExamplesAndBoundaries();
    TestRandomizedAgainstReference();
  } catch (const std::exception& error) {
    std::cerr << "CompressCoordinates test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "CompressCoordinates tests passed\n";
}
