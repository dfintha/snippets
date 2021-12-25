// bitwise_cast.cpp
// Bitwise casting between types.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <memory>

template <typename To, typename From>
constexpr To& bitwise_cast(From& from) {
    return *reinterpret_cast<To *>(std::addressof(from));
}

template <typename To, typename From>
constexpr const To& bitwise_cast(const From& from) {
    return *reinterpret_cast<const To *>(std::addressof(from));
}

// Demonstration

#include <cstdint>
#include <cstdio>

struct split {
    uint16_t a;
    uint8_t b;
    uint8_t c;
};

int main() {
    uint32_t from = 0x12345678;
    split to = bitwise_cast<split>(from);
    printf("from = 0x%X\n", from);
    printf("  to = 0x%X 0x%X 0x%X\n", to.a, to.b, to.c);
    return 0;
}
