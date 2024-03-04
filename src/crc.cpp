// crc.cpp
// Implementation of parametric CRC calculation as a C++ class template, similar
// to how std::hash works. Supports C++98. For more CRC parametrization options,
// see `crc.c` in the same directory where this file resides.
// Author: Dénes Fintha
// Year: 2024
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>

#if __cplusplus >= 201103L
#include <cstdint>
#include <type_traits>
#define STATIC_ASSERT(expression) static_assert(expression, #expression)
#else
#include <stdint.h>
#define STATIC_ASSERT(expression) ((void) 0)
#endif

template <typename UIntT>
UIntT reflect(UIntT value) {
    STATIC_ASSERT(std::is_integral<UIntT>::value && std::is_unsigned<UIntT>::value);
    static const uint8_t bits = sizeof(UIntT) * 8;
    UIntT result = 0x00;
    for (uint8_t i = 0; i < bits; ++i) {
        if ((value & (UIntT(0x01) << i)) != 0)
            result |= (UIntT(0x01) << (bits - 1 - i));
    }
    return result;
}

template <typename UIntT,
          UIntT Polynomial,
          UIntT Initial,
          UIntT OutputXOR,
          bool ReflectInput,
          bool ReflectOutput>
class crc {
public:
    UIntT operator()(const void *data, size_t length) {
        STATIC_ASSERT(std::is_integral<UIntT>::value && std::is_unsigned<UIntT>::value);

        if (length == 0)
            return Initial;

        UIntT result = Initial;
        static const UIntT bits = sizeof(UIntT) * 8;
        const uint8_t* bytes = reinterpret_cast<const uint8_t *>(data);
        for (size_t i = 0; i < length; ++i) {
            UIntT temp = ReflectInput ? reflect(bytes[i]) : bytes[i];
            result ^= (temp << (bits - 8));
            for (size_t j = 0; j < 8; ++j) {
                result = ((result & (UIntT(1) << (bits - 1))) != 0)
                    ? ((result << 1) ^ Polynomial)
                    : (result << 1);
            }
        }

        if (ReflectOutput)
            result = reflect(result);

        return result ^ OutputXOR;
    }
};

// Demonstration

#include <cstring>
#include <iomanip>
#include <iostream>

template <typename CRC>
void calculate_and_print(const char *text) {
    std::cout << std::uppercase << std::hex
              << uint64_t(CRC()(text, strlen(text))) << '\n';
}

int main() {
    const char *data = "123456789";
    std::cout << "Plain Text:    \"" << data << "\"\n";

    typedef crc<uint8_t,
                0x07,
                0x00,
                0x55,
                false,
                false> crc8itu;
    std::cout << "CRC-8/ITU:     0x";
    calculate_and_print<crc8itu>(data);

    typedef crc<uint16_t,
                0x8005,
                0xFFFF,
                0xFFFF,
                true,
                true> crc16usb;
    std::cout << "CRC-16/USB:    0x";
    calculate_and_print<crc16usb>(data);

    typedef crc<uint32_t,
                0x04C11DB7,
                0xFFFFFFFF,
                0x00000000,
                false,
                false> crc32mpeg2;
    std::cout << "CRC-32/MPEG-2: 0x";
    calculate_and_print<crc32mpeg2>(data);

    typedef crc<uint64_t,
                0x42F0E1EBA9EA3693,
                0xFFFFFFFFFFFFFFFF,
                0xFFFFFFFFFFFFFFFF,
                true,
                true> crc64ecma;
    std::cout << "CRC-64/ECMA:   0x";
    calculate_and_print<crc64ecma>(data);

    return 0;
}
