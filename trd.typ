#set document(title: "ICPC Team Reference Document")
#set page(paper: "a4", margin: 18mm, numbering: "1")
#set text(font: "Libertinus Serif", size: 10pt, lang: "ru")
#set heading(numbering: "1.1")
#show raw: set text(font: "DejaVu Sans Mono")
#show raw.where(block: true): set block(breakable: true)
#show raw.where(block: true): set par(leading: 0.45em)

#align(center)[
  #text(size: 18pt, weight: "bold")[ICPC Team Reference Document]
]

#outline(title: [Содержание], depth: 2)

= Структуры данных

#include "algorithms/fixed_set/description.typ"

#pagebreak(weak: true)

#include "algorithms/segment_tree/description.typ"

#pagebreak(weak: true)

= Графы

#include "algorithms/scc/description.typ"

#pagebreak(weak: true)

#include "algorithms/two_sat/description.typ"

#pagebreak(weak: true)

= Динамическое программирование

#[
  #show raw.where(block: true): set par(leading: 0.25em)
  #include "algorithms/knapsack_01/description.typ"
  #include "algorithms/reachable_sums/description.typ"
]
