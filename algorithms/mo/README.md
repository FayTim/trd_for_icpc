# Алгоритм Мо

Компактный раздел включён в общий PDF через `description.typ`.
C++20, индексы с нуля, отрезки `[l,r)`; запросы известны заранее.

| Файл | Назначение |
| --- | --- |
| `mo.h` | Змейка и Мо с изменениями |
| `rollback_mo.h` | Шаблон с откатами и частота моды |
| `range_statistics.h` | Частоты и k-я статистика на блоках по значениям |
| `approximate_lis.h` | Приближённый LIS через офлайн divide and conquer |
| `example.cpp` | Частоты, k-й элемент, LIS |
| `variants_example.cpp` | Изменения массива и откаты |

## Контракт

`Solve(n, queries, state, block_size=0)` вызывает `AddLeft/Right(i)`,
`RemoveLeft/Right(i)` и `Answer(id)`. Начальное окно пустое.
Ответы возвращаются во входном порядке; тип ответа создаётся по умолчанию
и допускает присваивание. `Answer` не меняет логическое состояние.
После запуска остаётся последнее окно; для нового запуска создайте пустое.

`SolveWithUpdates` дополнительно вызывает `Apply(id, forward, l, r)`.
Состояние начинает в исходной версии массива, изменение внутри окна
обновляет и массив, и статистику. `before` записывается на момент изменения.

`SolveRollback` требует `Snapshot()`, `Rollback(mark)` и быстрых
добавлений с обоих концов; в конце восстанавливает исходное состояние.

`RangeKth(a, queries)` использует **блоки по значениям, как в «Алгоритмике»**:
O(1) на обновление и O(√σ) на ответ. `KthQuery{l,r,k}` требует
`1 <= k <= r-l`; повторы учитываются, возвращается исходное значение.
Границы и размеры корректны, размеры помещаются в `int`.

В PDF также есть краткое сведение путей дерева к Мо и объяснение,
почему Мо с откатами не поддерживает LIS автоматически.

## Проверка

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure -R '^mo'
./build/algorithms/mo/mo_example
./build/algorithms/mo/mo_variants_example
```

Тесты работают в Release: сравнение с перебором, все k, откаты состояния
и времени, крайние int, 64-битные частоты; LIS на всех перестановках
до длины 7 и троичных массивах длины 7.

Источники: [KACTL](https://github.com/kth-competitive-programming/kactl/blob/main/content/data-structures/MoQueries.h),
[Алгоритмика](https://ru.algorithmica.org/cs/decomposition/mo/),
[разбор Infection Investigation, стр. 6](https://nerc.itmo.ru/archive/2025/northern/tutorial.pdf).
