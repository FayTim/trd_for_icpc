#include <iostream>
#include <string>

#include "polynomial_hashes.h"

int main() {
    const std::string text = "abacabaaba";
    const std::string pattern = "aba";
    for (int i : polynomial_hash::find_all(text, pattern)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';  // 0 4 7
}
