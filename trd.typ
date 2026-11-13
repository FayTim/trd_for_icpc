#set document(title: "ICPC Team Reference Document")
#set page(paper: "a4", flipped: true, columns: 2,
  margin: (x: 12mm, y: 12mm))
#set columns(gutter: 6mm)
#set text(font: "Libertinus Serif", size: 10pt, lang: "ru")
#set heading(numbering: "1.1")
#set par(leading: 0.45em, spacing: 0.6em)
#set list(spacing: 0.25em)
#show heading: set block(above: 0.85em, below: 0.4em)
#show raw.where(block: true): set block(above: 0.5em, below: 0.5em, breakable: true)
#set raw(theme: none)
#show raw: set text(font: "DejaVu Sans Mono")
#show raw.where(block: true): set par(leading: 0.25em)

#let marks = ("ШАБ", "СТ", "ГР", "ДП", "СТР", "ПРИ", "ЧИС", "КОМБ", "ГЕО")
#let current-chapter() = {
  let chapters = query(heading.where(level: 1, outlined: true)).filter(h =>
    h.location().page() <= here().page())
  if chapters.len() > 0 { chapters.last() } else { none }
}
#let chapter-number(h) = counter(heading).at(h.location()).first()
#let badge(i) = box(fill: black, inset: (x: 5pt, y: 2pt),
  text(fill: white, weight: "bold", size: 8pt)[#i #marks.at(i - 1)])
#set page(
  header: context {
    let h = current-chapter()
    if h != none {
      let i = chapter-number(h)
      grid(columns: (auto, 1fr), gutter: 6pt, badge(i),
        align(right, text(size: 8pt, h.body)))
    } else { text(size: 8pt)[ICPC TRD · карта блоков и оглавление] }
  },
  footer: context {
    let h = current-chapter()
    if h != none {
      let i = chapter-number(h)
      let local = here().page() - h.location().page() + 1
      let later = query(heading.where(level: 1, outlined: true)).filter(x =>
        x.location().page() > h.location().page())
      let last = if later.len() > 0 { later.first().location().page() - 1 }
        else { counter(page).final().first() }
      let total = last - h.location().page() + 1
      grid(columns: (1fr, 1fr),
        text(size: 8pt)[#marks.at(i - 1)-#local/#total],
        align(right, text(size: 8pt)[стр. #counter(page).display()]))
    } else { align(right, text(size: 8pt)[стр. #counter(page).display()]) }
  },
  background: context {
    let h = current-chapter()
    if h != none {
      let i = chapter-number(h)
      place(top + right, dx: -5mm, dy: 30mm + (i - 1) * 144mm / (marks.len() - 1),
        rect(width: 5mm, height: 12mm, fill: black, stroke: none, inset: 0pt)[
          #align(center + horizon, text(fill: white, size: 9pt, weight: "bold")[#i])
        ])
    }
  },
)

#align(center)[
  #text(size: 17pt, weight: "bold")[ICPC Team Reference Document]
]

*Карта блоков.* Номер на правом поле занимает постоянную высоту;
короткий код повторяется сверху и внизу каждой страницы.
Внизу слева, например `ГР-3/19`, — третья из 19 страниц блока;
справа — сквозная страница. В оглавлении адрес `3 ГР-3` означает блок
`3 ГР`, страницу 3 внутри него; после «стр.» указан сквозной номер.

#context table(columns: (auto, 1fr), inset: 3pt, stroke: 0.4pt,
  [*Метка*], [*Содержимое*],
  ..query(heading.where(level: 1, outlined: true)).map(h => {
    let i = chapter-number(h)
    ([#i #marks.at(i - 1)], h.body)
  }).flatten(),
)

*Перенос в решение.* Стандарт — C++20. Скопируйте нужный заголовок перед
`main`, сохранив его стандартные подключения. Локальные строки
`#include "...h"` замените самими напечатанными зависимостями в указанном
порядке; при объединении в один файл `#pragma once` не требуется.
Показанные примеры содержат отдельные `main`: выбирайте один и подставляйте
ввод своей задачи. Раздел «модификации» описывает изменения базового
алгоритма; применяйте их только при явно указанных предпосылках.

// Keep a chapter-local address and the global page number in each entry.
#show outline.entry: it => context {
  let target = it.element.location()
  let chapter = query(heading.where(level: 1, outlined: true))
    .filter(h => h.location().page() <= target.page()).last()
  let i = chapter-number(chapter)
  let local = target.page() - chapter.location().page() + 1
  let address = box(text(size: 8pt)[#i #marks.at(i - 1)-#local · стр. #target.page()])
  link(target, it.indented(it.prefix(), [
    #it.body()#h(0.5em)#box(width: 1fr, it.fill)#h(0.5em)#address
  ]))
}

#outline(title: [Содержание], depth: 2)

#pagebreak(weak: true)

= Шаблоны решения

#include "templates/description.typ"

#pagebreak(weak: true)

= Структуры данных

#include "algorithms/fixed_set/description.typ"


#include "algorithms/segment_tree/description.typ"


#include "algorithms/fenwick_tree/description.typ"


#pagebreak(weak: true)

= Графы

#include "algorithms/graphs/description.typ"


#include "algorithms/graphs/euler.typ"


#include "algorithms/scc/description.typ"


#include "algorithms/two_sat/description.typ"


#pagebreak(weak: true)

= Динамическое программирование

#include "algorithms/knapsack_01/description.typ"
#include "algorithms/reachable_sums/description.typ"
#include "algorithms/grouped_knapsack/description.typ"

#include "algorithms/bitmask_dp/description.typ"
#include "algorithms/tree_dp/description.typ"


#include "algorithms/interval_dp/description.typ"
#include "algorithms/knuth_dp/description.typ"


#pagebreak(weak: true)

= Алгоритмы на строки

#include "algorithms/z_function/description.typ"
#include "algorithms/polynomial_hashes/description.typ"


#pagebreak(weak: true)

= Алгоритмические приёмы

#include "algorithms/binary_search/description.typ"

#include "algorithms/gray_code/description.typ"

#include "algorithms/compress_coordinates/description.typ"


#pagebreak(weak: true)

= Теория чисел

#include "algorithms/number_theory/description.typ"


#pagebreak(weak: true)

= Комбинаторика

#include "algorithms/combinatorics/description.typ"

#pagebreak(weak: true)

= Геометрия

#include "algorithms/geometry/description.typ"


#include "algorithms/geometry/segments.typ"


#include "algorithms/geometry/rectangles.typ"


#include "algorithms/geometry/polygons.typ"
