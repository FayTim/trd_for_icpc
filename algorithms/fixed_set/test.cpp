#include "fixed_set.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

void ExpectContains(const FixedSet& set, int value, bool expected) {
  if (set.Contains(value) != expected) {
    throw std::runtime_error("Contains(" + std::to_string(value) +
                             ") should be " + (expected ? "true" : "false"));
  }
}

void CheckAgainstReference(const FixedSet& set, const std::vector<int>& input,
                           const std::vector<int>& queries) {
  const std::unordered_set<int> reference(input.begin(), input.end());
  for (int value : input) {
    ExpectContains(set, value, true);
  }
  for (int value : queries) {
    ExpectContains(set, value, reference.contains(value));
  }
}

void TestEmptyAndSingleton() {
  FixedSet set;
  const std::vector<int> probes{0, -1, 1, std::numeric_limits<int>::min(),
                               std::numeric_limits<int>::max()};
  CheckAgainstReference(set, {}, probes);
  set.Initialize({});
  CheckAgainstReference(set, {}, probes);

  for (int value : probes) {
    set.Initialize({value});
    CheckAgainstReference(set, {value}, probes);
  }

  set.Initialize(std::vector<int>(10000, -42));
  CheckAgainstReference(set, {-42}, {42, -41, -43, 0});
}

void TestDuplicatesAndExtremes() {
  const int low = std::numeric_limits<int>::min();
  const int high = std::numeric_limits<int>::max();
  const std::vector<int> input{low, high, low + 1, high - 1, 0, -1, 1,
                              -123456789, 123456789, low, high, 0, -1};
  FixedSet set;
  set.Initialize(input);
  CheckAgainstReference(set, input,
                        {low + 2, high - 2, -2, 2, 123456788, -123456788});
}

void TestReinitialization() {
  FixedSet set;
  const std::vector<int> probes{-100, -5, -1, 0, 1, 5, 100};
  for (const std::vector<int>& input :
       std::vector<std::vector<int>>{{-100, -1, 0, 1, 100}, {5, -5, 5}, {},
                                    {}, {0}, {-100, 100}, {}}) {
    set.Initialize(input);
    CheckAgainstReference(set, input, probes);
  }
}

void TestStructuredKeys() {
  std::vector<int> input;
  std::vector<int> queries;
  for (int value = -5000; value <= 5000; ++value) {
    queries.push_back(value);
    if (value % 3 == 0) {
      input.push_back(value);
    }
  }
  FixedSet set;
  set.Initialize(input);
  CheckAgainstReference(set, input, queries);

  input.clear();
  for (int bit = 0; bit < 31; ++bit) {
    input.push_back(1 << bit);
    input.push_back(-(1 << bit));
  }
  set.Initialize(input);
  CheckAgainstReference(set, input, queries);
}

void TestRandomizedAgainstReference() {
  std::mt19937 generator(0x5EED);
  std::uniform_int_distribution<int> full_range(
      std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
  std::uniform_int_distribution<int> small_range(-100, 100);
  FixedSet set;
  std::vector<int> previous_input;
  for (int trial = 0; trial < 100; ++trial) {
    std::vector<int> input;
    const int count = trial * 13;
    for (int index = 0; index < count; ++index) {
      input.push_back(trial % 2 == 0 ? full_range(generator)
                                    : small_range(generator));
    }
    std::vector<int> queries = previous_input;
    for (int index = 0; index < 2000; ++index) {
      queries.push_back(full_range(generator));
    }
    for (int value = -110; value <= 110; ++value) {
      queries.push_back(value);
    }
    set.Initialize(input);
    CheckAgainstReference(set, input, queries);
    previous_input = std::move(input);
  }
}

}  // namespace

int main() {
  try {
    TestEmptyAndSingleton();
    TestDuplicatesAndExtremes();
    TestReinitialization();
    TestStructuredKeys();
    TestRandomizedAgainstReference();
  } catch (const std::exception& error) {
    std::cerr << "FixedSet test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "FixedSet tests passed\n";
}
