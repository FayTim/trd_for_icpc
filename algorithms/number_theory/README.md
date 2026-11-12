# Простые числа и делимость

Раздел TRD для задач на простые числа, делители, взаимную простоту,
НОД/НОК и сравнения. [Описание и формулы](description.typ) подключены
в `trd.typ`; листинги читаются из проверяемых заголовков.

Все функции находятся в `nt`. Нужен C++20; факторизация и модульная
арифметика используют `__uint128_t` / `__int128_t` из GCC или Clang.

| Заголовок | Интерфейс и назначение |
| --- | --- |
| [sieve.h](sieve.h) | `Sieve(N)`: простые, SPF, функции Эйлера и Мёбиуса до N; `sieve.Factorize(n)`; `SegmentedPrimes(L, R)` на включительном отрезке |
| [factorization.h](factorization.h) | `IsPrime`, `Factorize` для `uint64_t`; `MulMod`, `PowMod`; `Divisors`, `Totient`, `DivisorCount`, `DivisorSum` по факторизации |
| [divisibility.h](divisibility.h) | Суммы и обращения по делителям/кратным, `ExactGcdPairs`, `Valuation`, `FactorialValuation`, `QuotientBlocks` |
| [modular.h](modular.h) | `ExtendedGcd`, `ModInverse`, `SolveCongruence`, `MergeCongruences` с невзаимно простыми модулями |

Факторизация представлена отсортированными парами `(простой, показатель)`.
Единица соответствует пустому списку; `Divisors({})` возвращает `{1}`.
Результат `Sieve::Factorize` можно передавать функциям из `factorization.h`.
`Divisors` не сортирует результат.

## Область применения и ограничения

- Для большого числа небольших входов используйте SPF. `Sieve` строится
  за O(N), хранит три массива `int` и список простых. При N = 10^7 нужно
  более 120 МБ. Если нужны только простые, уберите массивы phi/mu.
- Сегментированное решето хранит битовые таблицы до sqrt(R) и для [L, R],
  а также результат. Маленькая ширина отрезка не отменяет стоимость
  базового решета. Практический ориентир: R <= 10^12 и узкий отрезок.
- Miller-Rabin детерминирован на всём `uint64_t`. Pollard rho использует
  случайность только для скорости; каждая возвращённая факторизация точна.
  `Factorize` требует n >= 1, модуль в `MulMod`/`PowMod` положительный.
- `DivisorSum` требует, чтобы математический ответ помещался в `uint64_t`.
  Обратное к p-1 по модулю не требуется: геометрическая сумма считается
  напрямую, поэтому нет ограничения взаимной простоты знаменателя.
- Преобразования работают с индексами 1..A, игнорируют индекс 0 и требуют
  отсутствия переполнения промежуточных `int64_t` сумм. Для `ExactGcdPairs`
  частоты неотрицательны, их сумма <= 2^32. Считаются неупорядоченные пары
  различных индексов, в том числе пары равных значений.
- В `Valuation` число положительное, p простое. `FactorialValuation`
  допускает n = 0. `QuotientBlocks(0, ...)` не вызывает обработчик.
- `ExtendedGcd` принимает неотрицательные a,b, не оба нулевые. Модули
  положительны; CRT требует lcm(m,n) <= INT64_MAX. Знаковые остатки
  нормализуются, отсутствие решения возвращается как `std::nullopt`.

## Проверки

Четыре теста `number_theory_*` зарегистрированы в CTest. Они сравнивают
малые входы с полным перебором, проверяют тождества phi/mu, простые квадраты,
псевдопростые, 64-битные факторизации и модульную арифметику, а также
случайные массивы для точного НОД пар. Проверки работают в Release.
Для каждого заголовка есть компилируемый `*_example.cpp`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
typst compile --root . trd.typ build/document/trd.pdf
```

## Материалы

- [Линейное решето](https://cp-algorithms.com/algebra/prime-sieve-linear.html)
  и [сегментированное решето](https://cp-algorithms.com/algebra/sieve-of-eratosthenes.html).
- [Детерминированный Miller-Rabin](https://cp-algorithms.com/algebra/primality_tests.html)
  и [Pollard rho / Brent](https://cp-algorithms.com/algebra/factorization.html).
- [Функция Эйлера](https://cp-algorithms.com/algebra/phi-function.html),
  [функции делителей](https://cp-algorithms.com/algebra/divisors.html),
  [формула Лежандра](https://cp-algorithms.com/algebra/factorial-modulo.html).
- [Диофантовы уравнения](https://cp-algorithms.com/algebra/linear-diophantine-equation.html)
  и [CRT](https://cp-algorithms.com/algebra/chinese-remainder-theorem.html).
