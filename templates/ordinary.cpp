#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// Demo task: add two integers in [-10^9, 10^9]. Replace this function.
void Solve(std::istream& in, std::ostream& out) {
  long long a, b;
  if (!(in >> a >> b)) return;
  out << a + b << '\n';
}

void RunTests() {
  struct TestCase { std::string input, expected; };
  const TestCase tests[] = {
      {"2 3\n", "5\n"},
      {"-7\n4\n", "-3\n"},
      {"1000000000 1000000000", "2000000000\n"},
      {"", ""},
  };
  for (const auto& test : tests) {
    std::istringstream in(test.input);
    std::ostringstream out;
    Solve(in, out);
    if (out.str() != test.expected) {
      throw std::runtime_error("expected [" + test.expected +
                               "], got [" + out.str() + "]");
    }
  }
  std::cerr << "ordinary tests passed\n";
}

int main(int argc, char* argv[]) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  try {
    if (argc == 2 && std::string(argv[1]) == "--test") {
      RunTests();
    } else if (argc == 1) {
      Solve(std::cin, std::cout);
    } else {
      std::cerr << "Usage: solution [--test]\n";
      return 2;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
