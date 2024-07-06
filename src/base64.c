// base64.c
// Base64 encoding and decoding functions. Results shall be freed by the caller.
// In C++, an std::string or an std::vector<uint8_t> can be used to store the
// result of a conversion, and utilize RAII.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <stddef.h>
#include <stdint.h>

char * b64encode(const uint8_t *data, size_t length);
uint8_t * b64decode(const char *data);

int64_t b64toi64(const char *data);
uint64_t b64tou64(const char *data);
int32_t b64toi32(const char *data);
uint32_t b64tou32(const char *data);

// Implementation

#include <assert.h>
#include <stdlib.h>
#include <string.h>

char * b64encode(const uint8_t *data, size_t length) {
    static const char pad = '=';
    static const char *map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    const size_t chunks = length / 3;
    const size_t remainder = length % 3;

    char *result = (char *) malloc(chunks * 4 + ((remainder > 0) ? 4 : 0) + 1);
    char *where = result;

    for (size_t i = 0; i < chunks; ++i) {
        const uint8_t *chunk = &data[3 * i];
        *where++ = map[(chunk[0] & 0xFC) >> 2];
        *where++ = map[(chunk[0] & 0x03) << 4 | (chunk[1] >> 4)];
        *where++ = map[(chunk[1] & 0x0F) << 2 | (chunk[2] >> 6)];
        *where++ = map[(chunk[2] & 0x3F)];
    }

    if (remainder > 0) {
        uint8_t chunk[3] = {0, 0, 0};
        memcpy(&chunk, &data[chunks * 3], remainder);
        *where++ = map[(chunk[0] & 0xFC) >> 2];
        *where++ = map[(chunk[0] & 0x03) << 4 | (chunk[1] >> 4)];
        char c = map[(chunk[1] & 0x0F) << 2];
        c = (c == map[0]) ? pad : c;
        *where++ = c;
        *where++ = pad;
    }

    *where = '\0';
    return result;
}

static uint8_t decode_map(char c) {
    if (c >= 'A' && c <= 'Z')
        return c - 'A';

    if (c >= 'a' && c <= 'z')
        return c - 'a' + 26;

    if (c >= '0' && c <= '9')
        return c - '0' + 52;

    switch (c) {
        case '+': return 62;
        case '/': return 63;
    }

    return 0;
}

uint8_t * b64decode(const char *data) {
    static const char pad = '=';

    const size_t length = strlen(data);
    const size_t chunks = length / 4;

    uint8_t *result = (uint8_t *) malloc(chunks * 3 - 1);
    uint8_t *where = result;

    for (size_t i = 0; i < chunks; ++i) {
        const char *chunk = &data[4 * i];
        *where++ = (decode_map(chunk[0]) << 2) | (decode_map(chunk[1]) >> 4);
        if (chunk[2] != pad)
            *where++ = ((decode_map(chunk[1]) & 0x0F) << 4) | ((decode_map(chunk[2]) & 0x3C) >> 2);
        if (chunk[3] != pad)
            *where++ = ((decode_map(chunk[2]) & 0x03) << 6) | decode_map(chunk[3]);
    }

    return result;
}

#define DEFINE_B64_DECODE_FUNCTION(NAME, TYPE, DATALENGTH)      \
    TYPE NAME(const char *data) {                               \
        union {                                                 \
            TYPE to;                                            \
            uint8_t from[sizeof(TYPE)];                         \
        } converter;                                            \
        assert(strlen(data) == DATALENGTH);                     \
        uint8_t *bytes = b64decode(data);                       \
        memcpy(converter.from, bytes, sizeof(TYPE));            \
        free(bytes);                                            \
        return converter.to;                                    \
    }

DEFINE_B64_DECODE_FUNCTION(b64toi64, int64_t, 12)
DEFINE_B64_DECODE_FUNCTION(b64tou64, uint64_t, 12)
DEFINE_B64_DECODE_FUNCTION(b64toi32, int32_t, 8)
DEFINE_B64_DECODE_FUNCTION(b64tou32, uint32_t, 8)

#undef DEFINE_B64_DECODE_FUNCTION

// Demonstration

#include <inttypes.h>
#include <stdio.h>

static void test_base64(const char *original, const char *reference) {
    const size_t length = strlen(original);
    char *encoded = b64encode((const uint8_t *) original, length);
    char *decoded = (char *) b64decode(encoded);
    printf(
        "  Original: '%s'\n"
        "   Encoded: '%s'\n"
        " Reference: '%s'\n"
        "   Decoded: '%s'\n\n",
        original,
        encoded,
        reference,
        decoded
    );
    free(encoded);
    free(decoded);
}

#define test_base64_number(TYPE, VALUE, REFERENCE, FORMAT, FUNCTION)    \
    do {                                                                \
        const TYPE value = VALUE;                                       \
        const size_t length = sizeof(TYPE);                             \
        char *encoded = b64encode((const uint8_t *) &value, length);    \
        TYPE decoded = FUNCTION(encoded);                               \
        printf(                                                         \
            "  Original: %" FORMAT " (" #TYPE ")\n"                     \
            "   Encoded: '%s'\n"                                        \
            " Reference: '%s'\n"                                        \
            "   Decoded: %" FORMAT "\n\n",                              \
            value,                                                      \
            encoded,                                                    \
            REFERENCE,                                                  \
            decoded                                                     \
        );                                                              \
        free(encoded);                                                  \
    } while (0)

int main(void) {
    printf("Base-64 Tests\n\n");

    test_base64(
        "Hello, world!",
        "SGVsbG8sIHdvcmxkIQ=="
    );
    test_base64(
        "Many hands make light work.",
        "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"
    );
    test_base64(
        "light w",
        "bGlnaHQgdw=="
    );
    test_base64(
        "light wo",
        "bGlnaHQgd28="
    );
    test_base64(
        "light wor",
        "bGlnaHQgd29y"
    );

    test_base64_number(uint64_t, 0x1A1A1A1A1A1A1A1A, "GhoaGhoaGho=", PRIu64, b64tou64);
    test_base64_number(int64_t, 0x2B2B2B2B2B2B2B2B, "KysrKysrKys=", PRIi64, b64toi64);
    test_base64_number(uint32_t, 0x1A1A1A1A, "GhoaGg==", PRIu32, b64tou32);
    test_base64_number(int32_t, 0x2B2B2B2B, "KysrKw==", PRIi32, b64toi32);

    return EXIT_SUCCESS;
}
