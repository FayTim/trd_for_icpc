== TwoSat

2-SAT через SCC. `Solve()` — `O(n + m)` времени и памяти для `n` переменных
и `m` условий; добавление условия — амортизированное `O(1)`.

`TwoSat(n)` создаёт формулу для переменных `0..n-1`.
`Literal(i)` — `2*i` (переменная), `Literal(i, false)` — `2*i+1` (отрицание).
Методы принимают *литералы*; отрицание `a` — `a ^ 1`.

- `AddOr(a, b)` — `a || b`.
- `AddImplication(a, b)` — `a -> b`.
- `ForceTrue(a)` — `a == true`.
- `AddAtMostOne(a, b)` — `!a || !b`; оба ложных допустимы.
- `AddExactlyOne(a, b)` — `a != b`.
- `Solve()` — значения *переменных* или `std::nullopt`, если решения нет.
  При `n == 0` возвращает пустой вектор внутри `optional`.

=== Пример

#raw(read("example.cpp"), lang: "cpp", block: true)

=== Реализация

При копировании в один файл поместите `scc.h` выше и удалите
`#include "../scc/scc.h"`.

#raw(read("two_sat.h"), lang: "cpp", block: true)
