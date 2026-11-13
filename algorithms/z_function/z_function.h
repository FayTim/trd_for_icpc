#pragma once

#include <algorithm>
#include <vector>

template <typename T>
std::vector<int> z_function(const T& a) {
    const int n = static_cast<int>(a.size());
    std::vector<int> z(n, 0);
    if (n == 0) {
        return z;
    }

    z[0] = n;
    int l = 0;
    int r = 0;

    for (int i = 1; i < n; ++i) {
        if (i < r) {
            z[i] = std::min(r - i, z[i - l]);
        }
        while (i + z[i] < n && a[z[i]] == a[i + z[i]]) {
            ++z[i];
        }
        if (i + z[i] > r) {
            l = i;
            r = i + z[i];
        }
    }

    return z;
}