#include "z_function.h"

#include <iostream>
#include <string>

int main() {
    const std::string pattern = "aba";
    const std::string text = "abacabaaba";

    const std::vector<int> z = z_function(pattern + '#' + text);

    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
        if (z[pattern.size() + 1 + i] == static_cast<int>(pattern.size())) {
            std::cout << i << ' ';
        }
    }
    std::cout << '\n';  // 0 4 7
}