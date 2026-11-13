#include "two_sat.h"

#include <iostream>

int main() {
  TwoSat solver(2);
  const int x = TwoSat::Literal(0);
  const int y = TwoSat::Literal(1);

  solver.AddExactlyOne(x, y);
  solver.ForceTrue(TwoSat::Literal(0, false));

  if (const auto answer = solver.Solve(); answer.has_value()) {
    std::cout << std::boolalpha << (*answer)[0] << ' ' << (*answer)[1] << '\n';
    // false true: the result is indexed by variable, not by literal.
  } else {
    std::cout << "No solution\n";
  }

  solver.AddImplication(y, x);
  std::cout << solver.Solve().has_value() << '\n';  // false
}
