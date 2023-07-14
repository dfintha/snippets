/* md5.c
 * Implementation of the MD5 message-digest algorithm in ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t digest[4];
} md5_digest;

md5_digest md5(const void *data, size_t length);

/* Implementation */

#include <string.h>

static uint32_t switch_u32_endianness(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >>  8) |
           ((value & 0x0000FF00) <<  8) |
           ((value & 0x000000FF) << 24);
}

md5_digest md5(const void *data, size_t length) {
    static const uint8_t S[] = {
        7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
        5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
        4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
        6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
    };
    static const uint32_t K[] = {
        0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
        0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
        0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
        0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
        0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
        0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
        0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
        0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
        0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
        0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
        0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
        0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
        0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
        0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
        0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
        0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
    };

    md5_digest result = { 0x00, 0x00, 0x00, 0x00 };
    const uint32_t *M;
    const uint64_t bits = ((uint64_t) length) * 8;
    const size_t chunks = length / 64;
    const size_t remainder = length % 64;
    uint32_t last[16];
    uint32_t A = 0x67452301;
    uint32_t B = 0xEFCDAB89;
    uint32_t C = 0x98BADCFE;
    uint32_t D = 0x10325476;
    uint32_t AA, BB, CC, DD, F, g;
    size_t i;
    uint8_t j;
    union {
        uint8_t *as_bytes;
        uint32_t *as_dwords;
    } message;

    message.as_dwords = last;
    memcpy(message.as_bytes, ((const uint8_t *) data) + chunks * 64, remainder);
    i = remainder;
    message.as_bytes[i++] = 0x80;
    while (i % 56 != 0) {
        message.as_bytes[i++] = 0x00;
    }
    memcpy(message.as_bytes + i, &bits, sizeof(uint64_t));

    for (i = 0; i < chunks + 1; ++i) {
        if (i < chunks) {
            M = (const uint32_t*) data + (i * 16);
        } else {
            M = last;
        }

        AA = A;
        BB = B;
        CC = C;
        DD = D;

        for (j = 0; j < 64; ++j) {
            if (j < 16) {
                F = (BB & CC) | ((~BB) & DD);
                g = j;
            } else if (j < 32) {
                F = (DD & BB) | ((~DD) & CC);
                g = (5 * j + 1) % 16;
            } else if (j < 48) {
                F = BB ^ CC ^ DD;
                g = (3 * j + 5) % 16;
            } else {
                F = CC ^ (BB | (~DD));
                g = (7 * j) % 16;
            }

            F += AA + K[j] + M[g];
            AA = DD;
            DD = CC;
            CC = BB;
            BB += (F << S[j]) | (F >> (32 - S[j]));
        }

        A += AA;
        B += BB;
        C += CC;
        D += DD;
    }

    result.digest[0] = switch_u32_endianness(A);
    result.digest[1] = switch_u32_endianness(B);
    result.digest[2] = switch_u32_endianness(C);
    result.digest[3] = switch_u32_endianness(D);
    return result;
}

/* Demonstration */

#include <stdio.h>
#include <string.h>

static void perform_md5_test(
    const char *data,
    const char *reference
) {
    const size_t length = strlen(data);
    const md5_digest digest = md5(data, length);
    printf("      Text: '%s'\n", data);
    printf(
        "    Result: %04x%04x%04x%04x\n",
        digest.digest[0],
        digest.digest[1],
        digest.digest[2],
        digest.digest[3]
    );
    printf(" Reference: %s\n\n", reference);
}

int main(void) {
    perform_md5_test(
        "The quick brown fox jumps over the lazy dog",
        "9e107d9d372bb6826bd81d3542a419d6"
    );
    perform_md5_test(
        "The quick brown fox jumps over the lazy dog.",
        "e4d909c290d0fb1ca068ffaddf22cbd0"
    );
    perform_md5_test(
        "",
        "d41d8cd98f00b204e9800998ecf8427e"
    );
    return 0;
}
