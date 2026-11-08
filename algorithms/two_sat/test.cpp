#include "two_sat.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

enum class Operation { Or, Implication, Force, ExactlyOne };

struct Constraint {
  Operation operation;
  int a;
  int b;
};

void Check(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

bool LiteralValue(int literal, const std::vector<bool>& assignment) {
  const bool value = assignment[literal / 2];
  return literal % 2 == 0 ? value : !value;
}

bool Satisfied(const Constraint& constraint,
               const std::vector<bool>& assignment) {
  const bool a = LiteralValue(constraint.a, assignment);
  const bool b = LiteralValue(constraint.b, assignment);
  switch (constraint.operation) {
    case Operation::Or:
      return a || b;
    case Operation::Implication:
      return !a || b;
    case Operation::Force:
      return a;
    case Operation::ExactlyOne:
      return a != b;
  }
  throw std::runtime_error("Unknown operation");
}

bool Satisfied(const std::vector<Constraint>& constraints,
               const std::vector<bool>& assignment) {
  for (const Constraint& constraint : constraints) {
    if (!Satisfied(constraint, assignment)) {
      return false;
    }
  }
  return true;
}

void Apply(TwoSat& solver, const Constraint& constraint) {
  switch (constraint.operation) {
    case Operation::Or:
      solver.AddOr(constraint.a, constraint.b);
      break;
    case Operation::Implication:
      solver.AddImplication(constraint.a, constraint.b);
      break;
    case Operation::Force:
      solver.ForceTrue(constraint.a);
      break;
    case Operation::ExactlyOne:
      solver.AddExactlyOne(constraint.a, constraint.b);
      break;
  }
}

std::vector<bool> Assignment(int variables, unsigned mask) {
  std::vector<bool> assignment(variables);
  for (int variable = 0; variable < variables; ++variable) {
    assignment[variable] = ((mask >> variable) & 1U) != 0;
  }
  return assignment;
}

bool HasBruteForceSolution(int variables,
                          const std::vector<Constraint>& constraints) {
  for (unsigned mask = 0; mask < (1U << variables); ++mask) {
    if (Satisfied(constraints, Assignment(variables, mask))) {
      return true;
    }
  }
  return false;
}

void CheckSolver(const TwoSat& solver, int variables,
                 const std::vector<Constraint>& constraints,
                 const std::string& context) {
  const bool expected = HasBruteForceSolution(variables, constraints);
  // Repeated calls must preserve the formula; any satisfying model is valid.
  for (int repeat = 0; repeat < 2; ++repeat) {
    const auto answer = solver.Solve();
    Check(answer.has_value() == expected, context + ": wrong satisfiability");
    if (answer.has_value()) {
      Check(answer->size() == static_cast<std::size_t>(variables),
            context + ": wrong number of variable values");
      Check(Satisfied(constraints, *answer), context + ": invalid model");
    }
  }
}

void TestLiteralEncodingAndEmptyFormula() {
  for (int variable = 0; variable < 8; ++variable) {
    Check(TwoSat::Literal(variable) == 2 * variable, "Positive literal encoding");
    Check(TwoSat::Literal(variable, false) == 2 * variable + 1,
          "Negative literal encoding");
    Check((TwoSat::Literal(variable) ^ 1) == TwoSat::Literal(variable, false),
          "Literal negation");
  }
  for (int variables = 0; variables <= 8; ++variables) {
    const TwoSat solver(variables);
    CheckSolver(solver, variables, {}, "Unconstrained formula");
  }
  const auto empty = TwoSat(0).Solve();
  Check(empty.has_value() && empty->empty(), "Zero variables has an empty model");
}

void TestEveryHelperTruthTable() {
  const std::vector<Operation> operations{
      Operation::Or, Operation::Implication, Operation::Force,
      Operation::ExactlyOne};
  // Pin each assignment of two variables independently. This checks all signed
  // literal pairs, equal/complementary literals, and implication contraposition.
  for (Operation operation : operations) {
    for (int a = 0; a < 4; ++a) {
      for (int b = 0; b < 4; ++b) {
        for (unsigned mask = 0; mask < 4; ++mask) {
          TwoSat solver(2);
          std::vector<Constraint> constraints{{operation, a, b}};
          const auto assignment = Assignment(2, mask);
          for (int variable = 0; variable < 2; ++variable) {
            const int literal = TwoSat::Literal(variable, assignment[variable]);
            constraints.push_back({Operation::Force, literal, literal});
          }
          for (const auto& constraint : constraints) {
            Apply(solver, constraint);
          }
          CheckSolver(solver, 2, constraints,
                      "Helper truth table: operation=" +
                          std::to_string(static_cast<int>(operation)) +
                          " a=" + std::to_string(a) + " b=" +
                          std::to_string(b) + " mask=" + std::to_string(mask));
        }
      }
    }
  }
}

void TestRepeatedSolveWithNewConstraints() {
  TwoSat solver(3);
  std::vector<Constraint> constraints;
  CheckSolver(solver, 3, constraints, "Initially empty");
  const std::vector<Constraint> additions{
      {Operation::ExactlyOne, TwoSat::Literal(0), TwoSat::Literal(1)},
      {Operation::Implication, TwoSat::Literal(1), TwoSat::Literal(2)},
      {Operation::Force, TwoSat::Literal(0, false), TwoSat::Literal(0, false)},
      // The previous clauses now force x1=true and x2=true.
      {Operation::Force, TwoSat::Literal(2, false), TwoSat::Literal(2, false)},
      {Operation::Or, TwoSat::Literal(0), TwoSat::Literal(0, false)}};
  for (const Constraint& constraint : additions) {
    Apply(solver, constraint);
    constraints.push_back(constraint);
    CheckSolver(solver, 3, constraints, "Adding constraints after Solve");
  }
  Check(!solver.Solve().has_value(), "Contradictory formula must remain unsatisfiable");

  TwoSat contradiction(1);
  contradiction.ForceTrue(TwoSat::Literal(0));
  contradiction.ForceTrue(TwoSat::Literal(0, false));
  Check(!contradiction.Solve().has_value(), "Opposite forced literals contradict");
}

void TestRandomFormulasAgainstBruteForce() {
  std::mt19937 generator(0x2A7);
  std::uniform_int_distribution<int> operation_distribution(0, 3);
  std::uniform_int_distribution<int> count_distribution(0, 30);
  for (int variables = 1; variables <= 8; ++variables) {
    std::uniform_int_distribution<int> literal_distribution(0, 2 * variables - 1);
    for (int trial = 0; trial < 150; ++trial) {
      TwoSat solver(variables);
      std::vector<Constraint> constraints;
      const auto planted_model = Assignment(variables, generator());
      const int count = count_distribution(generator);
      const std::string context = "Random n=" + std::to_string(variables) +
                                  " trial=" + std::to_string(trial);
      for (int index = 0; index < count; ++index) {
        Constraint constraint;
        do {
          constraint = {static_cast<Operation>(operation_distribution(generator)),
                        literal_distribution(generator),
                        literal_distribution(generator)};
          // Half the formulas stay satisfiable, so model validation is exercised
          // even for longer formulas; the other half are unrestricted.
        } while (trial % 2 == 0 && !Satisfied(constraint, planted_model));
        Apply(solver, constraint);
        constraints.push_back(constraint);
        if (index % 5 == 0) {
          CheckSolver(solver, variables, constraints, context);
        }
      }
      CheckSolver(solver, variables, constraints, context);
    }
  }
}

}  // namespace

int main() {
  try {
    TestLiteralEncodingAndEmptyFormula();
    TestEveryHelperTruthTable();
    TestRepeatedSolveWithNewConstraints();
    TestRandomFormulasAgainstBruteForce();
  } catch (const std::exception& error) {
    std::cerr << "TwoSat test failed: " << error.what() << '\n';
    return 1;
  }
  std::cout << "TwoSat tests passed\n";
}
