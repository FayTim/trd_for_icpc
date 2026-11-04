# TRD for ICPC

Командный справочник алгоритмов и структур данных на C++20. Главный файл
[`trd.md`](trd.md) задаёт порядок разделов и подключает описания и исходники
из отдельных папок. PDF содержит тот же код, который проверяется тестами.

## Структура

```text
trd.md                         # содержание документа и подключения
algorithms/
  fixed_set/
    README.md                  # описание, ограничения и примеры
    fixed_set.h                # самостоятельная реализация
    test.cpp                   # тесты реализации
    CMakeLists.txt             # регистрация теста в CTest
CMakeLists.txt                 # подключение модулей с тестами
scripts/build_document.py      # раскрытие подключений и сборка PDF
document/                      # настройки оформления PDF
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

## Сборка документа

Нужны Python 3.9+, Pandoc, XeLaTeX и шрифты DejaVu. В Ubuntu / Debian:

```sh
sudo apt-get update
sudo apt-get install --no-install-recommends python3 pandoc texlive-xetex texlive-latex-extra texlive-lang-cyrillic fonts-dejavu lmodern
python3 scripts/build_document.py --pdf
```

Результат: `build/document/trd.pdf` и объединённый `build/document/trd.md`.
Для получения только Markdown достаточно Python:

```sh
python3 scripts/build_document.py
```

Сборщик раскрывает директивы, стоящие на отдельной строке:

- `{{ include: путь/к/описанию.md }}` — вставляет Markdown; вложенные подключения
  разрешены, циклические подключения считаются ошибкой.
- `{{ code: путь/к/реализации.h }}` — вставляет файл как блок кода C++.

Пути отсчитываются от файла с директивой. Исходники не нужно копировать в
описание. В `trd.md` заголовки разделов имеют уровень `#`, в README модуля —
`##`, а подразделы — `###`.

## Добавление алгоритма

1. Создайте `algorithms/<name>/` с реализацией, README, примерами и тестами.
   За образец можно взять [`fixed_set`](algorithms/fixed_set/README.md).
2. Добавьте `CMakeLists.txt` с исполняемой целью теста и `add_test`.
   Проверки должны работать и в Release: не полагайтесь на `assert`.
3. Подключите каталог через `add_subdirectory(algorithms/<name>)` в корневом
   `CMakeLists.txt`.
4. Добавьте в нужный раздел `trd.md` директивы `include` для README и `code`
   для реализации.
5. Запустите тесты и сборку PDF командами выше.

## CI

GitHub Actions запускает тесты и сборку PDF на каждом pull request и каждом
push в `main`, включая слияния PR. Задания независимы: PDF можно скачать из
артефакта `trd-pdf` у соответствующего запуска Actions. Для обязательного
прохождения проверок перед слиянием владелец репозитория может включить
required checks `Tests` и `PDF` в правилах ветки `main`.
