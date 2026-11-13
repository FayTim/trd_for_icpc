#pragma once

#include <chrono>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace polynomial_hash {

    constexpr uint64_t p = (1ULL << 61) - 1;

    inline uint64_t add(uint64_t a, uint64_t b) {
        a += b;
        return a >= p ? a - p : a;
    }

    inline uint64_t sub(uint64_t a, uint64_t b) {
        return a >= b ? a - b : a + p - b;
    }

    inline std::uint64_t mul(std::uint64_t a, std::uint64_t b) {
        return static_cast<__uint128_t>(a) * b % p;
    }

    inline std::uint64_t base() {
        static const std::uint64_t value =
            std::mt19937_64(chrono::steady_clock::now().time_since_epoch().count())() % (p - 256) + 256;
        return value;
    }

    // prefix[i] - хеш первых i символов, prefix[0] = 0.
    inline vector<uint64_t> build_prefix(const string& s) {
        vector<uint64_t> prefix(s.size() + 1, 0);
        for (size_t i = 0; i < s.size(); ++i) {
            // +1, чтобы символ '\0' в начале строки не терялся.
            prefix[i + 1] = add(mul(prefix[i], base()), static_cast<unsigned char>(s[i]) + 1);
        }
        return prefix;
    }

    // power[i] = base^i, i = 0..n.
    inline vector<uint64_t> build_powers(int n) {
        vector<uint64_t> power(n + 1, 1);
        for (int i = 1; i <= n; ++i) {
            power[i] = mul(power[i - 1], base());
        }
        return power;
    }

    // Хеш подстроки s[l, r), 0 <= l <= r <= |s|. Хеш пустой подстроки равен 0.
    inline uint64_t get(const vector<uint64_t>& prefix,
                             const vector<uint64_t>& power, int l, int r) {
        return sub(prefix[r], mul(prefix[l], power[r - l]));
    }

    // Все позиции с хешем pattern. Возможны коллизии; длины должны быть < INT_MAX.
    // Пустой шаблон совпадает во всех |text| + 1 границах строки.
    inline vector<int> find_all(const string& text, const string& pattern) {
        const int n = static_cast<int>(text.size());
        const int m = static_cast<int>(pattern.size());
        if (m > n) return {};
        const auto power = build_powers(n);
        const auto prefix = build_prefix(text);
        const auto pattern_prefix = build_prefix(pattern);
        const auto wanted = get(pattern_prefix, power, 0, m);
        vector<int> positions;
        for (int i = 0; i <= n - m; ++i) {
            if (get(prefix, power, i, i + m) == wanted) positions.push_back(i);
        }
        return positions;
    }

}
