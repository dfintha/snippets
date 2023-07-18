/* sha2_512.c
 * Implementation of the 64-bit SHA-2 cryptographic hash functions (SHA-384,
 * SHA-512, SHA-512/224, SHA-512/256 and the modified SHA-512 for generating
 * SHA-512/t hashes) in ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>
#include <stdint.h>

typedef enum {
    SHA384,
    SHA512,
    SHA512_224,
    SHA512_256,
    SHA512_MOD
} sha2_512_type;

typedef struct {
 uint64_t hash[8];
 sha2_512_type type;
} sha2_512_hash;

sha2_512_hash sha2_512(const void *data, size_t length, sha2_512_type type);

/* Implementation */

#include <string.h>

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

#define ror64(x, n) \
    ((x >> n) | (x << (64 - n)))

static void sha2_512_initialize(
    uint64_t *H0,
    uint64_t *H1,
    uint64_t *H2,
    uint64_t *H3,
    uint64_t *H4,
    uint64_t *H5,
    uint64_t *H6,
    uint64_t *H7,
    sha2_512_type type
) {
    sha2_512_hash subhash;

    if (type == SHA384) {
        *H0 = 0xCBBB9D5DC1059ED8;
        *H1 = 0x629A292A367CD507;
        *H2 = 0x9159015A3070DD17;
        *H3 = 0x152FECD8F70E5939;
        *H4 = 0x67332667FFC00B31;
        *H5 = 0x8EB44A8768581511;
        *H6 = 0xDB0C2E0D64F98FA7;
        *H7 = 0x47B5481DBEFA4FA4;
    } else if (type == SHA512) {
        *H0 = 0x6A09E667F3BCC908;
        *H1 = 0xBB67AE8584CAA73B;
        *H2 = 0x3C6EF372FE94F82B;
        *H3 = 0xA54FF53A5F1D36F1;
        *H4 = 0x510E527FADE682D1;
        *H5 = 0x9B05688C2B3E6C1F;
        *H6 = 0x1F83D9ABFB41BD6B;
        *H7 = 0x5BE0CD19137E2179;
    } else if (type == SHA512_MOD) {
        *H0 = 0x6A09E667F3BCC908 ^ 0xA5A5A5A5A5A5A5A5;
        *H1 = 0xBB67AE8584CAA73B ^ 0xA5A5A5A5A5A5A5A5;
        *H2 = 0x3C6EF372FE94F82B ^ 0xA5A5A5A5A5A5A5A5;
        *H3 = 0xA54FF53A5F1D36F1 ^ 0xA5A5A5A5A5A5A5A5;
        *H4 = 0x510E527FADE682D1 ^ 0xA5A5A5A5A5A5A5A5;
        *H5 = 0x9B05688C2B3E6C1F ^ 0xA5A5A5A5A5A5A5A5;
        *H6 = 0x1F83D9ABFB41BD6B ^ 0xA5A5A5A5A5A5A5A5;
        *H7 = 0x5BE0CD19137E2179 ^ 0xA5A5A5A5A5A5A5A5;
    } else if (type == SHA512_224) {
        subhash = sha2_512("SHA-512/224", 11, SHA512_MOD);
        *H0 = subhash.hash[0];
        *H1 = subhash.hash[1];
        *H2 = subhash.hash[2];
        *H3 = subhash.hash[3];
        *H4 = subhash.hash[4];
        *H5 = subhash.hash[5];
        *H6 = subhash.hash[6];
        *H7 = subhash.hash[7];
    } else if (type == SHA512_256) {
        subhash = sha2_512("SHA-512/256", 11, SHA512_MOD);
        *H0 = subhash.hash[0];
        *H1 = subhash.hash[1];
        *H2 = subhash.hash[2];
        *H3 = subhash.hash[3];
        *H4 = subhash.hash[4];
        *H5 = subhash.hash[5];
        *H6 = subhash.hash[6];
        *H7 = subhash.hash[7];
    } else {
        *H0 = *H1 = *H2 = *H3 = *H4 = *H5 = *H6 = *H7 = 0x00;
    }
}

sha2_512_hash sha2_512(const void *data, size_t length, sha2_512_type type) {
    const uint64_t K[80] = {
        0x428A2F98D728AE22, 0x7137449123EF65CD,
        0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC,
        0x3956C25BF348B538, 0x59F111F1B605D019,
        0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118,
        0xD807AA98A3030242, 0x12835B0145706FBE,
        0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2,
        0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1,
        0x9BDC06A725C71235, 0xC19BF174CF692694,
        0xE49B69C19EF14AD2, 0xEFBE4786384F25E3,
        0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
        0x2DE92C6F592B0275, 0x4A7484AA6EA6E483,
        0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5,
        0x983E5152EE66DFAB, 0xA831C66D2DB43210,
        0xB00327C898FB213F, 0xBF597FC7BEEF0EE4,
        0xC6E00BF33DA88FC2, 0xD5A79147930AA725,
        0x06CA6351E003826F, 0x142929670A0E6E70,
        0x27B70A8546D22FFC, 0x2E1B21385C26C926,
        0x4D2C6DFC5AC42AED, 0x53380D139D95B3DF,
        0x650A73548BAF63DE, 0x766A0ABB3C77B2A8,
        0x81C2C92E47EDAEE6, 0x92722C851482353B,
        0xA2BFE8A14CF10364, 0xA81A664BBC423001,
        0xC24B8B70D0F89791, 0xC76C51A30654BE30,
        0xD192E819D6EF5218, 0xD69906245565A910,
        0xF40E35855771202A, 0x106AA07032BBD1B8,
        0x19A4C116B8D2D0C8, 0x1E376C085141AB53,
        0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8,
        0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB,
        0x5B9CCA4F7763E373, 0x682E6FF3D6B2B8A3,
        0x748F82EE5DEFB2FC, 0x78A5636F43172F60,
        0x84C87814A1F0AB72, 0x8CC702081A6439EC,
        0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9,
        0xBEF9A3F7B2C67915, 0xC67178F2E372532B,
        0xCA273ECEEA26619C, 0xD186B8C721C0C207,
        0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178,
        0x06F067AA72176FBA, 0x0A637DC5A2C898A6,
        0x113F9804BEF90DAE, 0x1B710B35131C471B,
        0x28DB77F523047D84, 0x32CAAB7B40C72493,
        0x3C9EBE0A15C9BEBC, 0x431D67C49C100D4C,
        0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A,
        0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817
    };

    sha2_512_hash result;
    const uint64_t *M;
    const uint64_t bits = switch_u64_endianness(((uint64_t) length) * 8);
    const size_t chunks = length / 128;
    const size_t remainder = length % 128;
    uint64_t last[16];
    uint64_t W[80];
    uint64_t H0, H1, H2, H3, H4, H5, H6, H7;
    uint64_t S0, S1, T1, T2, A, B, C, D, E, F, G, H;
    size_t i;
    uint8_t j;
    union {
        uint8_t *as_bytes;
        uint64_t *as_qwords;
    } message;

    sha2_512_initialize(&H0, &H1, &H2, &H3, &H4, &H5, &H6, &H7, type);

    message.as_qwords = last;
    memcpy(message.as_bytes, ((const uint8_t *) data) + chunks * 128, remainder);
    i = remainder;
    message.as_bytes[i++] = 0x80;
    while (i % 112 != 0) {
        message.as_bytes[i++] = 0x00;
    }
    memset(message.as_bytes + i, 0x00, sizeof(uint64_t));
    i += sizeof(uint64_t);
    memcpy(message.as_bytes + i, &bits, sizeof(uint64_t));

    for (i = 0; i < chunks + 1; ++i) {
        if (i < chunks) {
            M = ((const uint64_t *) data) + (i * 16);
        } else {
            M = last;
        }

        for (j = 0; j < 80; ++j) {
            if (j < 16) {
                W[j] = switch_u64_endianness(M[j]);
            } else {
                S0 = ror64(W[j - 15],  1) ^ ror64(W[j - 15],  8) ^ (W[j - 15] >> 7);
                S1 = ror64(W[j -  2], 19) ^ ror64(W[j -  2], 61) ^ (W[j -  2] >> 6);
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

        for (j = 0; j < 80; ++j) {
            S1 = ror64(E, 14) ^ ror64(E, 18) ^ ror64(E, 41);
            T1 = H + S1 + ((E & F) ^ ((~E) & G)) + K[j] + W[j];
            S0 = ror64(A, 28) ^ ror64(A, 34) ^ ror64(A, 39);
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
    if (type == SHA384 || type == SHA512 || type == SHA512_MOD) {
        result.hash[4] = H4;
        result.hash[5] = H5;
    }
    if (type == SHA512 || type == SHA512_MOD) {
        result.hash[6] = H6;
        result.hash[7] = H7;
    }
    result.type = type;
    return result;
}

/* Demonstration */

#include <stdio.h>

static const char * sha2_512_type_to_string(sha2_512_type type) {
    switch (type) {
        case SHA384:
            return "SHA-384";
        case SHA512:
            return "SHA-512";
        case SHA512_224:
            return "SHA-512/224";
        case SHA512_256:
            return "SHA-512/256";
        case SHA512_MOD:
            return "SHA-512 (Modified)";
    }
    return "";
}

static void perform_sha2_512_test(
    const char *data,
    const char *reference,
    sha2_512_type type
) {
    const size_t length = strlen(data);
    const sha2_512_hash hash = sha2_512(data, length, type);
    const char *format;

    switch (type) {
        case SHA384:
            format = "    Result: %016lx%016lx%016lx%016lx%016lx%016lx\n";
            break;
        case SHA512:
        case SHA512_MOD:
            format = "    Result: %016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n";
            break;
        case SHA512_224:
            /* Only the left half of the fourth hash word is used! */
            format = "    Result: %016lx%016lx%016lx%08lx\n";
            break;
        case SHA512_256:
            format = "    Result: %016lx%016lx%016lx%016lx\n";
            break;
    }

    printf("      Mode: %s\n", sha2_512_type_to_string(type));
    printf("      Text: '%s'\n", data);
    printf(
        format,
        hash.hash[0],
        hash.hash[1],
        hash.hash[2],
        (type == SHA512_224) ? hash.hash[3] >> 32 : hash.hash[3],
        hash.hash[4],
        hash.hash[5],
        hash.hash[6],
        hash.hash[7]
    );
    printf(" Reference: %s\n\n", reference);
}

int main(void) {
    perform_sha2_512_test(
        "",
        "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
        "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
        SHA512
    );
    perform_sha2_512_test(
        "ABC",
        "397118fdac8d83ad98813c50759c85b8c47565d8268bf10da483153b747a7474"
        "3a58a90e85aa9f705ce6984ffc128db567489817e4092d050d8a1cc596ddc119",
        SHA512
    );
    perform_sha2_512_test(
        "The quick brown fox jumps over the lazy dog",
        "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb64"
        "2e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6",
        SHA512
    );
    perform_sha2_512_test(
        "The quick brown fox jumps over the lazy dog.",
        "91ea1245f20d46ae9a037a989f54f1f790f0a47607eeb8a14d12890cea77a1bb"
        "c6c7ed9cf205e67b7f2b8fd4c7dfd3a7a8617e45f3c463d481c7e586c39ac1ed",
        SHA512
    );
    perform_sha2_512_test(
        "The quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog The quick brown fox jumps over the lazy dog The "
        "quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog",
        "e489dcc2e8867d0bbeb0a35e6b94951a11affd7041ef39fa21719eb01800c29a"
        "2c3522924443939a7848fde58fb1dbd9698fece092c0c2b412c51a47602cfd38",
        SHA512
    );
    perform_sha2_512_test(
        "",
        "38b060a751ac96384cd9327eb1b1e36a21fdb71114be0743"
        "4c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
        SHA384
    );
    perform_sha2_512_test(
        "",
        "6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4",
        SHA512_224
    );
    perform_sha2_512_test(
        "",
        "c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a",
        SHA512_256
    );
    return 0;
}
