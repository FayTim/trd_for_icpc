#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <span>
#include <unordered_set>
#include <utility>
#include <vector>

class FixedSet {
 public:
  FixedSet() = default;

  void Initialize(const std::vector<int>& numbers) {
    std::unordered_set<int> data;
    data.insert(numbers.begin(), numbers.end());
    n_elements = data.size();
    tables.clear();
    tables.resize(n_elements);

    if (n_elements == 0) {
      return;
    }

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<std::uint64_t> multiplier(1, kPrime - 1);
    std::uniform_int_distribution<std::uint64_t> offset(0, kPrime - 1);

    std::vector<std::vector<int>> buckets(n_elements);
    while (true) {
      a = multiplier(generator);
      b = offset(generator);

      for (std::vector<int>& bucket : buckets) {
        bucket.clear();
      }

      for (const int n : data) {
        std::size_t i = hash_index(n, a, b, n_elements);
        buckets[i].push_back(n);
      }

      std::size_t bucket_size_squares_sum = 0;
      for (const std::span<const int> bucket : buckets) {
        bucket_size_squares_sum += bucket.size() * bucket.size();
      }

      if (bucket_size_squares_sum <= 4 * n_elements) {
        break;
      }
    }

    for (std::size_t bucket_index = 0; bucket_index < buckets.size();
         ++bucket_index) {
      const std::span<int> bucket = buckets[bucket_index];
      if (bucket.empty()) {
        continue;
      }

      std::size_t sz = bucket.size() * bucket.size();
      while (true) {
        std::uint64_t table_a = multiplier(generator);
        std::uint64_t table_b = offset(generator);

        bool collision = false;
        std::vector<std::optional<int>> slots(sz);
        for (const int n : bucket) {
          std::size_t i = hash_index(n, table_a, table_b, sz);

          if (slots[i].has_value()) {
            collision = true;
            break;
          }

          slots[i] = n;
        }

        if (!collision) {
          tables[bucket_index] =
              Table{.data = std::move(slots), .a = table_a, .b = table_b};
          break;
        }
      }
    }
  }

  bool Contains(int number) const {
    if (tables.empty()) {
      return false;
    }
    std::size_t table_index = hash_index(number, a, b, tables.size());
    return tables[table_index].Contains(number);
  }

 private:
  struct Table {
    std::vector<std::optional<int>> data;
    std::uint64_t a = 1, b = 0;

   public:
    bool Contains(int number) const {
      if (data.empty()) {
        return false;
      }

      std::size_t i = hash_index(number, a, b, data.size());
      return data[i] == number;
    }
  };

  std::size_t n_elements = 0;
  std::vector<Table> tables;
  std::uint64_t a = 1, b = 0;

  static constexpr std::uint64_t kPrime = 4294967311;

  static std::size_t hash_index(int key, std::uint64_t a, std::uint64_t b,
                                std::size_t size) {
    const std::int64_t wide_key = key;
    const std::uint64_t magnitude = wide_key < 0 ? -wide_key : wide_key;

    std::uint64_t residue = (a * magnitude) % kPrime;
    if (wide_key < 0 && residue != 0) {
      residue = kPrime - residue;
    }
    return ((residue + b) % kPrime) % size;
  }
};
