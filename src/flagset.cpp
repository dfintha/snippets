// flagset.cpp
// Bitwise flag set type in C++, with both template and function argument
// indexing. Template argument indexing has boundary checks implemented with
// static_assert.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <cstdint>

template <uint8_t Bits>
struct bitset {
    static_assert(Bits == 8 || Bits == 16 || Bits == 32 || Bits == 64);
    using type = uint8_t;
};

template <> struct bitset<8> {
    using type = uint8_t;
};

template <> struct bitset<16> {
    using type = uint16_t;
};

template <> struct bitset<32> {
    using type = uint32_t;
};

template <> struct bitset<64> {
    using type = uint64_t;
};

template <uint8_t Bits, uint8_t Index>
typename bitset<Bits>::type nth_mask() {
    static_assert(Index <= Bits && Index != 0);
    return typename bitset<Bits>::type(1) << (Index - 1);
}

template <uint8_t Bits>
typename bitset<Bits>::type nth_mask(uint8_t index) {
    return typename bitset<Bits>::type(1) << (index - 1);
}

template <uint8_t Bits, uint8_t Index>
typename bitset<Bits>::type full_mask() {
    return typename bitset<Bits>::type(-1);
}

template <uint8_t Bits>
class flagset {
public:
    using container_type = typename bitset<Bits>::type;

public:
    constexpr flagset(container_type source = 0) : container(source) {
    }

    constexpr flagset(const flagset&) = default;

    constexpr flagset(flagset&&) = default;

    constexpr flagset& operator=(container_type source) {
        container = source;
    }

    constexpr flagset& operator=(const flagset&) = default;

    constexpr flagset& operator=(flagset&&) = default;

    template <uint8_t Index>
    constexpr void set() {
        static_assert(Index <= Bits && Index != 0);
        container |= nth_mask<Bits, Index>();
    }

    constexpr void set(uint8_t index) {
        container |= nth_mask<Bits>(index);
    }

    template <uint8_t Index>
    constexpr void clear() {
        static_assert(Index <= Bits && Index != 0);
        container &= (~nth_mask<Bits, Index>());
    }

    constexpr void clear(uint8_t index) {
        container &= (~nth_mask<Bits>(index));
    }

    template <uint8_t Index>
    constexpr bool test() const {
        static_assert(Index <= Bits && Index != 0);
        return (container & nth_mask<Bits, Index>()) != 0;
    }

    constexpr bool test(uint8_t index) const {
        return (container & nth_mask<Bits>(index)) != 0;
    }

    constexpr void reset() {
        container = container_type(0);
    }

    static constexpr uint8_t length() {
        return Bits;
    }

    constexpr operator container_type() const {
        return container;
    }

private:
    container_type container;
};

// Demonstration

#include <cstdio>

template <uint8_t Bits>
static void print_flagset(const flagset<Bits>& flags) {
    for (char i = 1; i <= Bits; ++i) {
        printf("%d", flags.test(i));
    }
    printf("\n");
}

int main() {
    printf("12345678\n");
    printf("--------\n");
    flagset<8> flags;
    print_flagset(flags);
    flags.set(1);
    flags.set(3);
    flags.set<5>();
    flags.set<7>();
    print_flagset(flags);
    flags.clear(3);
    flags.clear<7>();
    print_flagset(flags);
    flags.reset();
    print_flagset(flags);
    return 0;
}
