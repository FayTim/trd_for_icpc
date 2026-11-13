#pragma once
using ll = long long;

// минимум унимодальной f на [l, r] (строго убывает, затем строго возрастает)
template <class F> double ternMin(double l, double r, F f, int it = 100) {
    while (it--) {
        double a = l + (r - l) / 3, b = r - (r - l) / 3;
        if (f(a) < f(b)) r = b; else l = a;
    }
    return (l + r) / 2;
}
// то же на целых [l, r]: бинпоиск по сравнению соседних значений
template <class F> ll ternMinInt(ll l, ll r, F f) {
    while (l < r) {
        ll m = l + (r - l) / 2;
        if (f(m) <= f(m + 1)) r = m; else l = m + 1;
    }
    return l;
}
