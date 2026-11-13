# TRD for ICPC

Командный справочник алгоритмов и структур данных на C++20.
Главный файл [`trd.typ`](trd.typ) задаёт порядок разделов и подключает
описания из отдельных папок. Документ собирается напрямую Typst;
листинги читаются из тех же файлов, которые компилируются в CI.

## Структура

```text
trd.typ                        # главный документ и подключения разделов
algorithms/
  fixed_set/
    description.typ            # описание, интерфейс и включение листингов
    fixed_set.h                # самостоятельная реализация
    example.cpp                # компилируемый пример использования
    test.cpp                   # тесты реализации
    CMakeLists.txt              # цели сборки и регистрация теста в CTest
  compress_coordinates/
    description.typ            # сжатие координат: описание и листинги
    compress_coordinates.h     # самостоятельная реализация
    example.cpp                # компилируемый пример использования
    test.cpp                   # тесты прямого и обратного отображения
    CMakeLists.txt              # цели сборки и регистрация теста в CTest
  scc/                         # компоненты сильной связности (Косарайю)
  two_sat/                     # 2-SAT на основе SCC
  number_theory/               # простые, делители, phi/mu, НОД и CRT
CMakeLists.txt                 # подключение модулей с тестами
.github/workflows/ci.yml        # тесты и PDF для PR и main
```

## Тесты

Нужны CMake 3.20 или новее и компилятор с поддержкой C++20, например GCC 12+
или Clang 15+.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Вместе с тестами компилируются примеры `fixed_set_example`,
`compress_coordinates_example`, `scc_example`
и `two_sat_example`. Модули [SCC](algorithms/scc/README.md) и
[2-SAT](algorithms/two_sat/README.md) содержат описание интерфейса и применения.
Раздел [теории чисел](algorithms/number_theory/README.md) включает четыре
самостоятельных заголовка, четыре теста и компилируемые примеры.
Для его 64-битной арифметики нужен GCC/Clang с `__uint128_t` и `__int128_t`.

## Сборка документа

Установите [Typst CLI](https://github.com/typst/typst#installation) 0.15.0
или новее; CI использует 0.15.1. Дополнительные пакеты и системные шрифты
не нужны: документ использует шрифты из поставки Typst.

```sh
mkdir -p build/document
typst compile --root . trd.typ build/document/trd.pdf
```

Для автоматической пересборки при редактировании:

```sh
typst watch --root . trd.typ build/document/trd.pdf
```

## Добавление алгоритма

1. Создайте `algorithms/<name>/` с реализацией, `description.typ`, примером
   и тестами. За образец можно взять [`fixed_set`](algorithms/fixed_set).
2. Добавьте `CMakeLists.txt` с исполняемой целью теста и `add_test`.
   Проверки должны работать и в Release: не полагайтесь на `assert`.
3. Подключите каталог через `add_subdirectory(algorithms/<name>)` в корневом
   `CMakeLists.txt`.
4. В нужный раздел `trd.typ` добавьте `#include "algorithms/<name>/description.typ"`.
   Описание модуля начинается с `==`, его подразделы — с `===`.
5. В описании подключайте код через `#raw(read("implementation.h"), lang: "cpp", block: true)`.
   Пути отсчитываются от файла описания; копировать исходник в документ не нужно.
6. Запустите тесты и сборку PDF командами выше.

## CI

GitHub Actions запускает тесты и сборку PDF на каждом pull request и каждом
push в `main`, включая слияния PR. Задания независимы: PDF можно скачать из
артефакта `trd-pdf` у соответствующего запуска Actions. Для обязательного
прохождения проверок перед слиянием владелец репозитория может включить
required checks `Tests` и `PDF` в правилах ветки `main`.
