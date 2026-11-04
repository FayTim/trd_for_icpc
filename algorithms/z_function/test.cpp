#include "z_function.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<int> Naive(const std::string& s) {
  const int n = static_cast<int>(s.size());
  std::vector<int> z(n, 0);
  for (int i = 0; i < n; ++i) {
    while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
      ++z[i];
    }
  }
  return z;
}

std::string ToString(const std::vector<int>& v) {
  std::string result;
  for (int x : v) {
    result += std::to_string(x) + ' ';
  }
  return result;
}

template <typename T>
void ExpectZ(const T& input, const std::vector<int>& expected,
             const std::string& name) {
  const std::vector<int> actual = z_function(input);
  if (actual != expected) {
    throw std::runtime_error(name + ": expected [" + ToString(expected) +
                             "], got [" + ToString(actual) + "]");
  }
}

void TestKnownValues() {
  ExpectZ(std::string(), {}, "empty");
  ExpectZ(std::string("a"), {1}, "single");
  ExpectZ(std::string("aaaaa"), {5, 4, 3, 2, 1}, "all equal");
  ExpectZ(std::string("abcd"), {4, 0, 0, 0}, "all different");
  ExpectZ(std::string("abacaba"), {7, 0, 1, 0, 3, 0, 1}, "abacaba");
  ExpectZ(std::string("abcabc"), {6, 0, 0, 3, 0, 0}, "abcabc");
}

void TestOtherContainers() {
  ExpectZ(std::vector<int>{1, 1, 2, 1, 1, 2, 1}, {7, 1, 0, 4, 1, 0, 1},
          "vector<int>");
  ExpectZ(std::vector<int>{-1, 0, -1, 0}, {4, 0, 2, 0}, "negative ints");
}

void TestRandomizedAgainstNaive() {
  std::mt19937 generator(0x5EED);
  for (int trial = 0; trial < 20000; ++trial) {
    const int length = static_cast<int>(generator() % 40) + 1;
    const int alphabet = static_cast<int>(generator() % 3) + 1;
    std::string s(length, 'a');
    for (char& c : s) {
      c = static_cast<char>('a' + generator() % alphabet);
    }
    ExpectZ(s, Naive(s), "random \"" + s + "\"");
  }
}

void TestSubstringSearch() {
  std::mt19937 generator(0xC0FFEE);
  for (int trial = 0; trial < 2000; ++trial) {
    std::string text(generator() % 60 + 1, 'a');
    std::string pattern(generator() % 4 + 1, 'a');
    for (char& c : text) c = static_cast<char>('a' + generator() % 2);
    for (char& c : pattern) c = static_cast<char>('a' + generator() % 2);

    const std::vector<int> z = z_function(pattern + '#' + text);
    std::vector<int> found;
    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
      if (z[pattern.size() + 1 + i] == static_cast<int>(pattern.size())) {
        found.push_back(i);
      }
    }

    std::vector<int> expected;
    for (std::size_t pos = text.find(pattern); pos != std::string::npos;
         pos = text.find(pattern, pos + 1)) {
      expected.push_back(static_cast<int>(pos));
    }
    if (found != expected) {
      throw std::runtime_error("search failed: text=" + text +
                               " pattern=" + pattern);
    }
  }
}

}

int main() {
  try {
    TestKnownValues();
    TestOtherContainers();
    TestRandomizedAgainstNaive();
    TestSubstringSearch();
  } catch (const std::exception& error) {
    std::cerr << "z_function test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "z_function tests passed\n";
}