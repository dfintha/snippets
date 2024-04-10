// base64.c
// Base64 encoding and decoding functions. Results shall be freed by the caller.
// In C++, an std::string or an std::vector<uint8_t> can be used to store the
// result of a conversion, and utilize RAII. Also, an std::map can be used for
// the lookup during decoding.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <stddef.h>
#include <stdint.h>

char * b64encode(const uint8_t *data, size_t length);
uint8_t * b64decode(const char *data);

// Implementation

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

// Demonstration

#include <stdio.h>

int main(void) {
    const char *hello_original = "Hello, world!";
    const char *hello_reference = "SGVsbG8sIHdvcmxkIQ==";
    char *hello_encoded = b64encode((uint8_t *) hello_original, strlen(hello_original));
    uint8_t *hello_decoded = b64decode(hello_encoded);
    printf("%s -E-> %s (should be %s)\n", hello_original, hello_encoded, hello_reference);
    printf("%s -D-> %s (should be %s)\n", hello_encoded, (char *) hello_decoded, hello_original);
    free(hello_encoded);
    free(hello_decoded);
    return EXIT_SUCCESS;
}
