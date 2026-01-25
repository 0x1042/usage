#include <cstdint>

#include <immintrin.h>

struct U128 {
    uint64_t low = 0;
    uint64_t high = 0;
};

auto operator+(U128 a, U128 b) -> U128;
auto operator-(U128 a, U128 b) -> U128;
auto operator*(U128 a, U128 b) -> U128;
auto operator==(U128 a, U128 b) -> bool;
auto operator<(U128 a, U128 b) -> bool;

inline auto operator==(U128 a, U128 b) -> bool {
    return a.low == b.low && a.high == b.high;
}

inline auto operator<(U128 a, U128 b) -> bool {
    uint64_t dont_care = 0;

    // compute borrow from (a.low - b.low). If a.low < b.low => borrow = 1.
    unsigned char borrow = _subborrow_u64(0, a.low, b.low, &dont_care);

    // now subtract highs with that borrow
    // final borrow tells us if a < b in 128-bit unsigned.
    borrow = _subborrow_u64(borrow, a.high, b.high, &dont_care);

    return borrow != 0;
}

inline auto operator+(U128 a, U128 b) -> U128 {
    U128 r;
    unsigned char c = _addcarry_u64(0, a.low, b.low, &r.low);
    (void)_addcarry_u64(c, a.high, b.high, &r.high);
    return r;
}

inline auto operator-(U128 a, U128 b) -> U128 {
    U128 r;
    unsigned char c = _subborrow_u64(0, a.low, b.low, &r.low);
    (void)_subborrow_u64(c, a.high, b.high, &r.high);
    return r;
}

inline auto operator*(U128 a, U128 b) -> U128 {
    U128 r;

    uint64_t p0_hi = 0;
    r.low = _mulx_u64(a.low, b.low, &p0_hi);

    // cross terms: only the low 64 bits contribute to r.high
    uint64_t t1_hi = 0;
    uint64_t t1_lo = _mulx_u64(a.low, b.high, &t1_hi);

    uint64_t t2_hi = 0;
    uint64_t t2_lo = _mulx_u64(a.high, b.low, &t2_hi);

    // simply add is sufficient: carries would land in bit 128 and are discarded
    r.high = p0_hi + t1_lo + t2_lo;

    return r;
}
