#include <bit>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>

enum class Reply { Less, Equal, Greater };

// Demo: the judge chose a secret in [1, n]. Reply compares secret with guess.
template <class Ask>
long long FindSecret(long long n, Ask ask) {
  if (n < 1) throw std::runtime_error("n must be positive");
  const int budget = std::bit_width(static_cast<unsigned long long>(n));
  int queries = 0;
  long long lo = 1, hi = n;
  while (true) {
    if (queries == budget) throw std::runtime_error("query limit exceeded");
    ++queries;
    const long long mid = std::midpoint(lo, hi);
    const Reply reply = ask(mid);
    if (reply == Reply::Equal) return mid;
    if (reply == Reply::Less) {
      if (mid == lo) throw std::runtime_error("inconsistent judge reply");
      hi = mid - 1;
    } else {
      if (mid == hi) throw std::runtime_error("inconsistent judge reply");
      lo = mid + 1;
    }
  }
}

struct StdioJudge {
  std::istream& in;
  std::ostream& out;

  Reply operator()(long long guess) {
    out << "? " << guess << '\n' << std::flush;
    std::string reply;
    if (!(in >> reply) || reply == "ERROR") {
      throw std::runtime_error("judge stopped the interaction");
    }
    if (reply == "LT") return Reply::Less;
    if (reply == "EQ") return Reply::Equal;
    if (reply == "GT") return Reply::Greater;
    throw std::runtime_error("unknown judge reply");
  }
};

void RunTests() {
  const auto check = [](long long n, long long secret) {
    int queries = 0;
    const auto ask = [&](long long guess) {
      if (guess < 1 || guess > n) throw std::runtime_error("illegal query");
      ++queries;
      return secret < guess ? Reply::Less
           : secret > guess ? Reply::Greater : Reply::Equal;
    };
    if (FindSecret(n, ask) != secret ||
        queries > std::bit_width(static_cast<unsigned long long>(n))) {
      throw std::runtime_error("incorrect answer or query count");
    }
  };
  for (long long n = 1; n <= 96; ++n) {
    for (long long secret = 1; secret <= n; ++secret) check(n, secret);
  }
  for (long long n : {127LL, 128LL, 129LL,
                      std::numeric_limits<long long>::max()}) {
    for (long long secret : {1LL, n / 2, n}) check(n, secret);
  }
  for (const std::string input : {"", "ERROR\n", "unexpected\n"}) {
    std::istringstream in(input);
    std::ostringstream out;
    bool stopped = false;
    try { (void)FindSecret(1, StdioJudge{in, out}); }
    catch (const std::runtime_error&) { stopped = true; }
    if (!stopped || out.str() != "? 1\n") {
      throw std::runtime_error("interaction must stop after an error");
    }
  }
  bool stopped = false;
  try { (void)FindSecret(1, [](long long) { return Reply::Greater; }); }
  catch (const std::runtime_error&) { stopped = true; }
  if (!stopped) throw std::runtime_error("inconsistent reply accepted");
  std::cerr << "interactive tests passed\n";
}

int main(int argc, char* argv[]) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  try {
    if (argc == 2 && std::string(argv[1]) == "--test") {
      RunTests();
    } else if (argc == 1) {
      long long n;
      if (!(std::cin >> n)) return 0;
      const long long answer = FindSecret(n, StdioJudge{std::cin, std::cout});
      std::cout << "! " << answer << '\n' << std::flush;
    } else {
      std::cerr << "Usage: solution [--test]\n";
      return 2;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
