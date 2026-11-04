#include <iostream>
#include <string>

#include "polynomial_hashes.h"

int main() {
    const std::string text = "abacabaaba";
    const std::string pattern = "aba";
    const int n = static_cast<int>(text.size());
    const int m = static_cast<int>(pattern.size());

    const auto power = polynomial_hash::build_powers(n);
    const auto text_prefix = polynomial_hash::build_prefix(text);
    const auto pattern_prefix = polynomial_hash::build_prefix(pattern);
    const auto pattern_hash = polynomial_hash::get(pattern_prefix, power, 0, m);

    for (int i = 0; i + m <= n; ++i) {
        if (polynomial_hash::get(text_prefix, power, i, i + m) == pattern_hash) {
            std::cout << i << ' ';
        }
    }
    std::cout << '\n';  // 0 4 7
}
