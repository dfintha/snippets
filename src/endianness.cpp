// endianness.cpp
// Endianness utilities for integer values for C++98 and above.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

#if __cplusplus > 201103
#define CONSTEXPR constexpr
#define NOEXCEPT noexcept
#else
#define CONSTEXPR
#define NOEXCEPT throw()
#endif

#if __cplusplus > 201500
#define CONSTEXPR17 constexpr
#else
#define CONSTEXPR17
#endif

#include <stdint.h>

enum endianness_t {
    little_endian,
    big_endian
};

inline endianness_t get_endianness_runtime() NOEXCEPT {
    static union {
        uint16_t u16;
        uint8_t u8s[2];
    } test = { 0x00FF };
    static endianness_t result = (test.u8s[0] == 0xFF)
        ? little_endian
        : big_endian;
    return result;
}

inline CONSTEXPR17 endianness_t get_endianness() NOEXCEPT {
#if defined(__BIG_ENDIAN__)
    return big_endian;
#elif defined(__LITTLE_ENDIAN__)
    return little_endian;
#elif defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return big_endian;
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return little_endian;
#else
    return get_endianness_runtime();
#endif
#else
    return get_endianness_runtime();
#endif
}

inline CONSTEXPR uint16_t switch_endianness(uint16_t value) NOEXCEPT {
    return ((value & 0xFF00) >> 8) |
           ((value & 0x00FF) << 8);
}

inline CONSTEXPR uint32_t switch_endianness(uint32_t value) NOEXCEPT {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >>  8) |
           ((value & 0x0000FF00) <<  8) |
           ((value & 0x000000FF) << 24);
}

inline CONSTEXPR uint64_t switch_endianness(uint64_t value) NOEXCEPT {
    return ((value & 0xFF00000000000000) >> 56) |
           ((value & 0x00FF000000000000) >> 40) |
           ((value & 0x0000FF0000000000) >> 24) |
           ((value & 0x000000FF00000000) >>  8) |
           ((value & 0x00000000FF000000) <<  8) |
           ((value & 0x0000000000FF0000) << 24) |
           ((value & 0x000000000000FF00) << 40) |
           ((value & 0x00000000000000FF) << 56);
}

template <typename IntT>
CONSTEXPR17 IntT switch_endianness(IntT value) NOEXCEPT {
    if (sizeof(value) == 1)
        return value;
    uint8_t *bytes = reinterpret_cast<uint8_t *>(&value);
    for (unsigned i = 0; i < sizeof(value) / 2; ++i) {
        const uint8_t temp = bytes[i];
        bytes[i] = bytes[sizeof(value) - i - 1];
        bytes[sizeof(value) - i - 1] = temp;
    }
    return value;
}

template <typename IntT>
CONSTEXPR IntT LE(IntT value) NOEXCEPT {
    return (get_endianness() == big_endian)
        ? switch_endianness(value)
        : value;
}

template <typename IntT>
CONSTEXPR IntT BE(IntT value) NOEXCEPT {
    return (get_endianness() == little_endian)
        ? switch_endianness(value)
        : value;
}

// Demonstration

#include <cstdio>

static const char *format_endianness(endianness_t endianness) {
    return endianness == big_endian ? "big-endian" : "little-endian";
}

template <typename T>
void dump(const T& value) {
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&value);
    for (unsigned i = 0; i < sizeof(value); ++i) {
        printf("%02X ", unsigned(bytes[i]));
    }
    putc('\n', stdout);
}

int main() {
    #define DEFINE_VALUE(bits, number)      \
        uint##bits##_t value = number;      \
        unsigned value_bits = bits;         \
        const char *value_str = #number
    DEFINE_VALUE(64, 0xAABBCCDDEEFF0720);
    #undef DEFINE_VALUE
    printf(
        "The system has %s architecture (potential compile-time check).\n"
        "The system has %s architecture (runtime check).\n"
        "\n"
        "The following hexdumps are made from the unsigned %u-bit\n"
        "representation of the number %s.\n"
        "\n",
        format_endianness(get_endianness()),
        format_endianness(get_endianness_runtime()),
        value_bits,
        value_str
    );
    fputs("        Native: ", stdout);
    dump(value);
    fputs(" Little-Endian: ", stdout);
    dump(LE(value));
    fputs("    Big-Endian: ", stdout);
    dump(BE(value));
    return 0;
}
