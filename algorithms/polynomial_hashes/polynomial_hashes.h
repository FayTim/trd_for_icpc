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

}