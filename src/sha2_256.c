/* sha2_256.c
 * Implementation of the 32-bit SHA-2 cryptographic hash functions (SHA-224 and
 * SHA-256 in ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>
#include <stdint.h>

typedef enum {
    SHA224,
    SHA256
} sha2_256_type;

typedef struct {
 uint32_t hash[8];
 sha2_256_type type;
} sha2_256_hash;

sha2_256_hash sha2_256(const void *data, size_t length, sha2_256_type type);

/* Implementation */

#include <string.h>

#define switch_u32_endianness(value)            \
    (                                           \
        ((value & 0xFF000000) >> 24) |          \
        ((value & 0x00FF0000) >>  8) |          \
        ((value & 0x0000FF00) <<  8) |          \
        ((value & 0x000000FF) << 24)            \
    )

#define switch_u64_endianness(value)            \
    (                                           \
        ((value & 0xFF00000000000000) >> 56) |  \
        ((value & 0x00FF000000000000) >> 40) |  \
        ((value & 0x0000FF0000000000) >> 24) |  \
        ((value & 0x000000FF00000000) >>  8) |  \
        ((value & 0x00000000FF000000) <<  8) |  \
        ((value & 0x0000000000FF0000) << 24) |  \
        ((value & 0x000000000000FF00) << 40) |  \
        ((value & 0x00000000000000FF) << 56)    \
    )

#define ror32(x, n) \
    ((x >> n) | (x << (32 - n)))

sha2_256_hash sha2_256(const void *data, size_t length, sha2_256_type type) {
    const uint32_t K[64] = {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
        0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
        0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
        0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
        0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
        0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
        0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
    };

    sha2_256_hash result;
    const uint32_t *M;
    const uint64_t bits = switch_u64_endianness(((uint64_t) length) * 8);
    const size_t chunks = length / 64;
    const size_t remainder = length % 64;
    uint32_t last[16];
    uint32_t W[64];
    uint32_t H0 = (type == SHA224) ? 0xC1059ED8 : 0x6A09E667;
    uint32_t H1 = (type == SHA224) ? 0x367CD507 : 0xBB67AE85;
    uint32_t H2 = (type == SHA224) ? 0x3070DD17 : 0x3C6EF372;
    uint32_t H3 = (type == SHA224) ? 0xF70E5939 : 0xA54FF53A;
    uint32_t H4 = (type == SHA224) ? 0xFFC00B31 : 0x510E527F;
    uint32_t H5 = (type == SHA224) ? 0x68581511 : 0x9B05688C;
    uint32_t H6 = (type == SHA224) ? 0x64F98FA7 : 0x1F83D9AB;
    uint32_t H7 = (type == SHA224) ? 0xBEFA4FA4 : 0x5BE0CD19;
    uint32_t S0, S1, T1, T2, A, B, C, D, E, F, G, H;
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

        for (j = 0; j < 64; ++j) {
            if (j < 16) {
                W[j] = switch_u32_endianness(M[j]);
            } else {
                S0 = ror32(W[j - 15],  7) ^ ror32(W[j - 15], 18) ^ (W[j - 15] >>  3);
                S1 = ror32(W[j -  2], 17) ^ ror32(W[j -  2], 19) ^ (W[j -  2] >> 10);
                W[j] = W[j - 16] + S0 + W[j - 7] + S1;
            }
        }

        A = H0;
        B = H1;
        C = H2;
        D = H3;
        E = H4;
        F = H5;
        G = H6;
        H = H7;

        for (j = 0; j < 64; ++j) {
            S1 = ror32(E, 6) ^ ror32(E, 11) ^ ror32(E, 25);
            T1 = H + S1 + ((E & F) ^ ((~E) & G)) + K[j] + W[j];
            S0 = ror32(A, 2) ^ ror32(A, 13) ^ ror32(A, 22);
            T2 = S0 + ((A & B) ^ (A & C) ^ (B & C));
            H = G;
            G = F;
            F = E;
            E = D + T1;
            D = C;
            C = B;
            B = A;
            A = T1 + T2;
        }

        H0 += A;
        H1 += B;
        H2 += C;
        H3 += D;
        H4 += E;
        H5 += F;
        H6 += G;
        H7 += H;
    }

    result.hash[0] = H0;
    result.hash[1] = H1;
    result.hash[2] = H2;
    result.hash[3] = H3;
    result.hash[4] = H4;
    result.hash[5] = H5;
    result.hash[6] = H6;
    if (type == SHA256)
        result.hash[7] = H7;
    result.type = type;
    return result;
}

/* Demonstration */

#include <stdio.h>

static void perform_sha2xx_test(
    const char *data,
    const char *reference,
    sha2_256_type type
) {
    const size_t length = strlen(data);
    const sha2_256_hash hash = sha2_256(data, length, type);
    const char *format = (type == SHA224)
        ? "    Result: %08x%08x%08x%08x%08x%08x%08x\n"
        : "    Result: %08x%08x%08x%08x%08x%08x%08x%08x\n";
    printf("      Mode: %s\n", (type == SHA224) ? "SHA-224" : "SHA-256");
    printf("      Text: '%s'\n", data);
    printf(
        format,
        hash.hash[0],
        hash.hash[1],
        hash.hash[2],
        hash.hash[3],
        hash.hash[4],
        hash.hash[5],
        hash.hash[6],
        hash.hash[7]
    );
    printf(" Reference: %s\n\n", reference);
}

int main(void) {
    perform_sha2xx_test(
        "",
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        SHA256
    );
    perform_sha2xx_test(
        "ABC",
        "b5d4045c3f466fa91fe2cc6abe79232a1a57cdf104f7a26e716e0a1e2789df78",
        SHA256
    );
    perform_sha2xx_test(
        "The quick brown fox jumps over the lazy dog",
        "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592",
        SHA256
    );
    perform_sha2xx_test(
        "The quick brown fox jumps over the lazy dog.",
        "ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c",
        SHA256
    );
    perform_sha2xx_test(
        "The quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog The quick brown fox jumps over the lazy dog The "
        "quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog",
        "86c55ba51d6b4aef51f4ae956077a0f661d0b876c5774fef3172c4f56092cbbd",
        SHA256
    );
    perform_sha2xx_test(
        "",
        "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
        SHA224
    );
    perform_sha2xx_test(
        "The quick brown fox jumps over the lazy dog",
        "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525",
        SHA224
    );
    perform_sha2xx_test(
        "The quick brown fox jumps over the lazy dog.",
        "619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c",
        SHA224
    );
    return 0;
}
