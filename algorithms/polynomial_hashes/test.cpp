#include "polynomial_hashes.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace ph = polynomial_hash;

void Expect(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

std::string RandomString(std::mt19937& generator, int length, int alphabet) {
  std::string s(length, 'a');
  for (char& c : s) {
    c = static_cast<char>('a' + generator() % alphabet);
  }
  return s;
}

// Хеш всей строки напрямую.
std::uint64_t Whole(const std::string& s) {
  const auto prefix = ph::build_prefix(s);
  const auto power = ph::build_powers(static_cast<int>(s.size()));
  return ph::get(prefix, power, 0, static_cast<int>(s.size()));
}

void TestEmptyAndSpecialChars() {
  Expect(Whole("") == 0, "empty hash should be 0");

  const auto prefix = ph::build_prefix("abc");
  const auto power = ph::build_powers(3);
  for (int i = 0; i <= 3; ++i) {
    Expect(ph::get(prefix, power, i, i) == 0, "empty substring should have hash 0");
  }

  Expect(Whole(std::string("\0a", 2)) != Whole("a"), "\\0 prefix must matter");
}

void TestSubstringsMatchDirectHash() {
  std::mt19937 generator(0x5EED);
  for (int trial = 0; trial < 200; ++trial) {
    const int n = static_cast<int>(generator() % 40) + 1;
    const std::string s = RandomString(generator, n, 3);
    const auto prefix = ph::build_prefix(s);
    const auto power = ph::build_powers(n);
    for (int l = 0; l <= n; ++l) {
      for (int r = l; r <= n; ++r) {
        Expect(ph::get(prefix, power, l, r) == Whole(s.substr(l, r - l)),
               "substring hash mismatch in \"" + s + "\"");
      }
    }
  }
}

void TestSearchAgainstFind() {
  std::mt19937 generator(0xC0FFEE);
  for (int trial = 0; trial < 2000; ++trial) {
    const std::string text =
        RandomString(generator, static_cast<int>(generator() % 60) + 1, 2);
    const std::string pattern =
        RandomString(generator, static_cast<int>(generator() % 4) + 1, 2);
    const int n = static_cast<int>(text.size());
    const int m = static_cast<int>(pattern.size());

    const auto power = ph::build_powers(n);
    const auto text_prefix = ph::build_prefix(text);
    const std::uint64_t pattern_hash = Whole(pattern);

    std::vector<int> found;
    for (int i = 0; i + m <= n; ++i) {
      if (ph::get(text_prefix, power, i, i + m) == pattern_hash) {
        found.push_back(i);
      }
    }
    std::vector<int> expected;
    for (std::size_t pos = text.find(pattern); pos != std::string::npos;
         pos = text.find(pattern, pos + 1)) {
      expected.push_back(static_cast<int>(pos));
    }
    Expect(found == expected, "search mismatch: text=" + text + " pattern=" + pattern);
  }
}

void TestLongString() {
  const std::string s(200000, 'a');
  const auto prefix = ph::build_prefix(s);
  const auto power = ph::build_powers(200000);
  Expect(ph::get(prefix, power, 0, 100000) == ph::get(prefix, power, 100000, 200000),
         "equal halves should have equal hashes");
  Expect(ph::get(prefix, power, 0, 100000) != ph::get(prefix, power, 0, 99999),
         "different lengths");
}

}

int main() {
  try {
    TestEmptyAndSpecialChars();
    TestSubstringsMatchDirectHash();
    TestSearchAgainstFind();
    TestLongString();
  } catch (const std::exception& error) {
    std::cerr << "polynomial_hash test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "polynomial_hash tests passed\n";
}