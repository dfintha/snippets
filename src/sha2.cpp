// sha2.cpp
// Implementation of the SHA-2 cryptographic hash functions (SHA-224, SHA-256,
// SHA-384 and SHA-512) in C++11. Note that this only works on little-endian
// systems, as the constants' endianness is not converted. This will be refined.
// Author: Dénes Fintha
// Year: 2024
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

uint32_t switch_endianness(uint32_t value);
uint64_t switch_endianness(uint64_t value);
uint32_t rotate_right(uint32_t x, uint32_t n);
uint64_t rotate_right(uint64_t x, uint64_t n);

template <typename ChunkT, const ChunkT Init[8],
          size_t KCount, const ChunkT K[KCount],
          bool UseH6, bool UseH7>
class sha2 {
    static constexpr bool is_u32 = std::is_same<ChunkT, uint32_t>::value;
    static constexpr bool is_u64 = std::is_same<ChunkT, uint64_t>::value;
    static constexpr bool is_k64 = (KCount == 64);
    static constexpr bool is_k80 = (KCount == 80);
    static_assert((is_u32 && is_k64) || (is_u64 && is_k80),
                  "invalid underlying type or k-count");

    static constexpr ChunkT  R1 = (is_u32) ?  7 :  1;
    static constexpr ChunkT  R2 = (is_u32) ? 18 :  8;
    static constexpr ChunkT  R3 = (is_u32) ?  3 :  7;
    static constexpr ChunkT  R4 = (is_u32) ? 17 : 19;
    static constexpr ChunkT  R5 = (is_u32) ? 19 : 61;
    static constexpr ChunkT  R6 = (is_u32) ? 10 :  6;
    static constexpr ChunkT  R7 = (is_u32) ?  6 : 14;
    static constexpr ChunkT  R8 = (is_u32) ? 11 : 18;
    static constexpr ChunkT  R9 = (is_u32) ? 25 : 41;
    static constexpr ChunkT R10 = (is_u32) ?  2 : 28;
    static constexpr ChunkT R11 = (is_u32) ? 13 : 34;
    static constexpr ChunkT R12 = (is_u32) ? 22 : 39;

public:
    struct hash_t {
        ChunkT chunks[8];
    };

    static constexpr size_t chunk_count = 8 - (UseH7 ? 0 : 1) - (UseH6 ? 0 : 1);

    hash_t operator()(const void *data, size_t length) {
        constexpr size_t two_chunk_bits = sizeof(ChunkT) * 16;
        const uint64_t bits = switch_endianness(((uint64_t) length) * 8);
        const size_t chunks = length / two_chunk_bits;
        const size_t remainder = length % two_chunk_bits;

        // Initialize the hash chunks.
        ChunkT H0 = Init[0];
        ChunkT H1 = Init[1];
        ChunkT H2 = Init[2];
        ChunkT H3 = Init[3];
        ChunkT H4 = Init[4];
        ChunkT H5 = Init[5];
        ChunkT H6 = Init[6];
        ChunkT H7 = Init[7];

        // Pad the message.
        union {
            uint8_t *as_bytes;
            ChunkT *as_chunks;
        } message;
        ChunkT last[16];
        message.as_chunks = last;
        memcpy(
            message.as_bytes,
            ((const uint8_t *) data) + chunks * two_chunk_bits,
            remainder
        );
        size_t i = remainder;
        message.as_bytes[i++] = 0x80;
        while (i % (two_chunk_bits - sizeof(ChunkT) * 2) != 0) {
            message.as_bytes[i++] = 0x00;
        }
        if (is_u64) {
            memset(message.as_bytes + i, 0x00, sizeof(uint64_t));
            i += sizeof(uint64_t);
        }
        memcpy(message.as_bytes + i, &bits, sizeof(uint64_t));

        // Perform the hash function on the chunks.
        for (i = 0; i < chunks + 1; ++i) {
            const ChunkT *M = (i < chunks)
                ? ((const ChunkT*) data + (i * 16))
                : last;

            ChunkT S0, S1;
            ChunkT W[KCount];
            for (uint8_t j = 0; j < KCount; ++j) {
                if (j < 16) {
                    W[j] = switch_endianness(M[j]);
                } else {
                    S0 = rotate_right(W[j - 15], R1);
                    S0 ^= rotate_right(W[j - 15], R2);
                    S0 ^= (W[j - 15] >> R3);
                    S1 = rotate_right(W[j -  2], R4);
                    S1 ^= rotate_right(W[j -  2], R5);
                    S1 ^= (W[j -  2] >> R6);
                    W[j] = W[j - 16] + S0 + W[j - 7] + S1;
                }
            }

            ChunkT A = H0;
            ChunkT B = H1;
            ChunkT C = H2;
            ChunkT D = H3;
            ChunkT E = H4;
            ChunkT F = H5;
            ChunkT G = H6;
            ChunkT H = H7;

            for (uint8_t j = 0; j < KCount; ++j) {
                S1 = rotate_right(E, R7);
                S1 ^= rotate_right(E, R8);
                S1 ^= rotate_right(E, R9);
                const ChunkT T1 = H + S1 + ((E & F) ^ ((~E) & G)) + K[j] + W[j];
                S0 = rotate_right(A, R10);
                S0 ^= rotate_right(A, R11);
                S0 ^= rotate_right(A, R12);
                const ChunkT T2 = S0 + ((A & B) ^ (A & C) ^ (B & C));
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

        return hash_t {{ H0, H1, H2, H3, H4, H5, H6, H7 }};
    }
};

constexpr uint32_t sha224_init[8] = {
    0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939,
    0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4
};

constexpr uint32_t sha256_init[8] = {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

constexpr uint64_t sha384_init[8] = {
    0xCBBB9D5DC1059ED8, 0x629A292A367CD507,
    0x9159015A3070DD17, 0x152FECD8F70E5939,
    0x67332667FFC00B31, 0x8EB44A8768581511,
    0xDB0C2E0D64F98FA7, 0x47B5481DBEFA4FA4
};

constexpr uint64_t sha512_init[8] = {
    0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
    0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
    0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
    0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179,
};

constexpr uint32_t sha2_32bit_k[64] = {
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

constexpr uint64_t sha2_64bit_k[80] = {
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

using sha224 = sha2<uint32_t, sha224_init, 64, sha2_32bit_k, true, false>;
using sha256 = sha2<uint32_t, sha256_init, 64, sha2_32bit_k, true, true>;
using sha384 = sha2<uint64_t, sha384_init, 80, sha2_64bit_k, false, false>;
using sha512 = sha2<uint64_t, sha512_init, 80, sha2_64bit_k, true, true>;

// Implementation

uint32_t switch_endianness(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >>  8) |
           ((value & 0x0000FF00) <<  8) |
           ((value & 0x000000FF) << 24);
}

uint64_t switch_endianness(uint64_t value) {
    return ((value & 0xFF00000000000000) >> 56) |
           ((value & 0x00FF000000000000) >> 40) |
           ((value & 0x0000FF0000000000) >> 24) |
           ((value & 0x000000FF00000000) >>  8) |
           ((value & 0x00000000FF000000) <<  8) |
           ((value & 0x0000000000FF0000) << 24) |
           ((value & 0x000000000000FF00) << 40) |
           ((value & 0x00000000000000FF) << 56);
}

uint32_t rotate_right(uint32_t x, uint32_t n) {
    return ((x >> n) | (x << (32 - n)));
}

uint64_t rotate_right(uint64_t x, uint64_t n) {
    return ((x >> n) | (x << (64 - n)));
}

// Demonstration

#include <cstdio>

void print_chunk(uint32_t chunk) {
    printf("%08x", chunk);
}

void print_chunk(uint64_t chunk) {
    printf("%016lx", chunk);
}

template <typename SHA2Hasher>
void sha2_test(
    const char *data,
    const char *reference,
    const char *type,
    SHA2Hasher hasher
) {
    const size_t length = strlen(data);
    const auto hash = hasher(data, length);
    printf("      Mode: %s\n", type);
    printf("      Text: '%s'\n", data);
    printf("    Result: ");
    for (size_t i = 0; i < SHA2Hasher::chunk_count; ++i) {
        print_chunk(hash.chunks[i]);
    }
    printf("\n Reference: %s\n\n", reference);
}

int main() {
    sha2_test(
        "",
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "SHA-256",
        sha256()
    );
    sha2_test(
        "ABC",
        "b5d4045c3f466fa91fe2cc6abe79232a1a57cdf104f7a26e716e0a1e2789df78",
        "SHA-256",
        sha256()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog",
        "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592",
        "SHA-256",
        sha256()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog.",
        "ef537f25c895bfa782526529a9b63d97aa631564d5d789c2b765448c8635fb6c",
        "SHA-256",
        sha256()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog The quick brown fox jumps over the lazy dog The "
        "quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog",
        "86c55ba51d6b4aef51f4ae956077a0f661d0b876c5774fef3172c4f56092cbbd",
        "SHA-256",
        sha256()
    );
    sha2_test(
        "",
        "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
        "SHA-224",
        sha224()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog",
        "730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525",
        "SHA-224",
        sha224()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog.",
        "619cba8e8e05826e9b8c519c0a5c68f4fb653e8a3d8aa04bb2c8cd4c",
        "SHA-224",
        sha224()
    );
    sha2_test(
        "",
        "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
        "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
        "SHA-512",
        sha512()
    );
    sha2_test(
        "ABC",
        "397118fdac8d83ad98813c50759c85b8c47565d8268bf10da483153b747a7474"
        "3a58a90e85aa9f705ce6984ffc128db567489817e4092d050d8a1cc596ddc119",
        "SHA-512",
        sha512()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog",
        "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb64"
        "2e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6",
        "SHA-512",
        sha512()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog.",
        "91ea1245f20d46ae9a037a989f54f1f790f0a47607eeb8a14d12890cea77a1bb"
        "c6c7ed9cf205e67b7f2b8fd4c7dfd3a7a8617e45f3c463d481c7e586c39ac1ed",
        "SHA-512",
        sha512()
    );
    sha2_test(
        "The quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog The quick brown fox jumps over the lazy dog The "
        "quick brown fox jumps over the lazy dog The quick brown fox jumps "
        "over the lazy dog",
        "e489dcc2e8867d0bbeb0a35e6b94951a11affd7041ef39fa21719eb01800c29a"
        "2c3522924443939a7848fde58fb1dbd9698fece092c0c2b412c51a47602cfd38",
        "SHA-512",
        sha512()
    );
    sha2_test(
        "",
        "38b060a751ac96384cd9327eb1b1e36a21fdb71114be0743"
        "4c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
        "SHA-384",
        sha384()
    );
    return 0;
}
