#pragma once

#include <optional>
#include <vector>

#include "../scc/scc.h"

class TwoSat {
 public:
  explicit TwoSat(int variables) : graph_(2 * variables) {}

  // x_i -> 2*i; !x_i -> 2*i+1. Отрицание литерала: literal ^ 1.
  static int Literal(int variable, bool positive = true) {
    return 2 * variable + (positive ? 0 : 1);
  }

  // a OR b.
  void AddOr(int a, int b) {
    graph_[a ^ 1].push_back(b);
    graph_[b ^ 1].push_back(a);
  }

  // a -> b. Добавляет также контрапозицию !b -> !a.
  void AddImplication(int a, int b) { AddOr(a ^ 1, b); }

  // Литерал a должен быть истинным.
  // Чтобы x_i был ложным: ForceTrue(Literal(i, false)).
  void ForceTrue(int a) { AddOr(a, a); }

  // Истинен ровно один из литералов a, b.
  void AddExactlyOne(int a, int b) {
    AddOr(a, b);
    AddOr(a ^ 1, b ^ 1);
  }

  // nullopt — решения нет; иначе answer[i] — значение ПЕРЕМЕННОЙ x_i.
  std::optional<std::vector<bool>> Solve() const {
    const auto scc = FindScc(graph_);
    const int variables = static_cast<int>(graph_.size() / 2);
    std::vector<bool> answer(variables);

    for (int i = 0; i < variables; ++i) {
      const int positive = Literal(i);
      const int negative = positive ^ 1;

      if (scc.component[positive] == scc.component[negative]) {
        return std::nullopt;
      }

      answer[i] = scc.component[positive] > scc.component[negative];
    }

    return answer;
  }

 private:
  std::vector<std::vector<int>> graph_;
};
