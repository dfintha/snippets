/* chacha20.c
 * Implementation of the ChaCha20 encryption algorithm (based on RFC 7539) in
 * ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>
#include <stdint.h>

void chacha20_block(
    const void *key,
    const void *nonce,
    const uint32_t counter,
    uint32_t *output
);

void chacha20_encrypt(
    const void *key,
    const void *nonce,
    const uint32_t counter,
    const void *data_in,
    void *data_out,
    size_t length
);

/* Implementation */

#include <string.h>

#define ROL32(x, n) \
    (((x) << (n)) | ((x) >> (32 - (n))))

#define CHACHA20_QR(a, b, c, d)         \
    a += b; d ^= a; d = ROL32(d, 16);   \
    c += d; b ^= c; b = ROL32(b, 12);   \
    a += b; d ^= a; d = ROL32(d,  8);   \
    c += d; b ^= c; b = ROL32(b,  7)

void chacha20_block(
    const void *key,
    const void *nonce,
    const uint32_t counter,
    uint32_t *output
) {
    static const uint32_t constants[4] = {
        0x61707865,
        0x3320646E,
        0x79622D32,
        0x6B206574
    };

    size_t i;
    uint32_t work[16];
    uint32_t state[16];

    memcpy(state + 0, constants, 4 * sizeof(uint32_t));
    memcpy(state + 4, key, 8 * sizeof(uint32_t));
    memcpy(state + 12, &counter, 1 * sizeof(uint32_t));
    memcpy(state + 13, nonce, 3 * sizeof(uint32_t));
    memcpy(work, state, 16 * sizeof(uint32_t));

    for (i = 0; i < 10; ++i) {
        CHACHA20_QR(work[0], work[4], work[ 8], work[12]);
        CHACHA20_QR(work[1], work[5], work[ 9], work[13]);
        CHACHA20_QR(work[2], work[6], work[10], work[14]);
        CHACHA20_QR(work[3], work[7], work[11], work[15]);
        CHACHA20_QR(work[0], work[5], work[10], work[15]);
        CHACHA20_QR(work[1], work[6], work[11], work[12]);
        CHACHA20_QR(work[2], work[7], work[ 8], work[13]);
        CHACHA20_QR(work[3], work[4], work[ 9], work[14]);
    }

    for (i = 0; i < 16; ++i) {
        state[i] += work[i];
        output[i] = state[i];
    }
}

void chacha20_encrypt(
    const void *key,
    const void *nonce,
    const uint32_t counter,
    const void *data_in,
    void *data_out,
    size_t length
) {
    const uint8_t *plaintext = (const uint8_t *) data_in;
    uint8_t *encrypted_message = (uint8_t *) data_out;

    size_t i, j, k;
    const uint8_t *block;
    union {
        uint32_t as_words[16];
        uint8_t as_bytes[64];
    } key_stream;

    k = 0;
    for (j = 0; j < length / 64; ++j) {
        chacha20_block(key, nonce, counter + j, key_stream.as_words);
        block = plaintext + (j * 64);
        for (i = 0; i < 64; ++i) {
            encrypted_message[k++] = key_stream.as_bytes[i] ^ block[i];
        }
    }

    if (length % 64 != 0) {
        chacha20_block(key, nonce, counter + j, key_stream.as_words);
        block = plaintext + (j * 64);
        for (i = 0; i < length % 64; ++i) {
            encrypted_message[k++] = key_stream.as_bytes[i] ^ block[i];
        }
    }
}

/* Demonstration */

#include <inttypes.h>
#include <stdio.h>

static void test_chacha20_block(void) {
    static const uint32_t counter = 0x00000001;
    static const uint32_t nonce[3] = { 0x09000000, 0x4A000000, 0x00000000 };
    static const uint32_t key[8] = {
        0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C,
        0x13121110, 0x17161514, 0x1B1A1918, 0x1F1E1D1C
    };
    static const uint32_t expected[16] = {
        0xE4E7F110, 0x15593BD1, 0x1FDD0F50, 0xC47120A3,
        0xC7F4D1C7, 0x0368C033, 0x9AAA2204, 0x4E6CD4C3,
        0x466482D2, 0x09AA9F07, 0x05D7C214, 0xA2028BD9,
        0xD19C12B5, 0xB94E16DE, 0xE883D0CB, 0x4E3C50A2
    };

    int i, j;
    uint32_t result[16];

    chacha20_block(key, nonce, counter, result);

    printf("--- TESTING CHACHA20 BLOCK FUNCTION ---\n\n");
    for (i = 0; i < 4; ++i) {
        printf("R[%02d..%02d]: ", i * 4, i * 4 + 3);
        for (j = 0; j < 4; ++j) {
            printf("0x%08" PRIX32 " ", result[i * 4 + j]);
        }
        printf("\nE[%02d..%02d]: ", i * 4, i * 4 + 3);
        for (j = 0; j < 4; ++j) {
            printf("0x%08" PRIX32 " ", expected[i * 4 + j]);
        }
        printf("\n           ");
        for (j = 0; j < 4; ++j) {
            if (result[i * 4 + j] != expected[i * 4 + j]) {
                printf("^^^^^^^^^^ ");
            } else {
                printf("           ");
            }
        }
        putc('\n', stdout);
    }
}

static void test_chacha20_encrypt(void) {
    static const char plaintext[114] =
        "Ladies and Gentlemen of the class of '99: "
        "If I could offer you only one tip for the future, "
        "sunscreen would be it.";
    static const size_t length = 114;
    static const uint32_t counter = 0x00000001;
    static const uint32_t nonce[3] = { 0x00000000, 0x4A000000, 0x00000000 };
    static const uint32_t key[8] = {
        0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C,
        0x13121110, 0x17161514, 0x1B1A1918, 0x1F1E1D1C
    };
    static const uint8_t expected[114] = {
        0x6E, 0x2E, 0x35, 0x9A, 0x25, 0x68, 0xF9, 0x80,
        0x41, 0xBA, 0x07, 0x28, 0xDD, 0x0D, 0x69, 0x81,
        0xE9, 0x7E, 0x7A, 0xEC, 0x1D, 0x43, 0x60, 0xC2,
        0x0A, 0x27, 0xAF, 0xCC, 0xFD, 0x9F, 0xAE, 0x0B,
        0xF9, 0x1B, 0x65, 0xC5, 0x52, 0x47, 0x33, 0xAB,
        0x8F, 0x59, 0x3D, 0xAB, 0xCD, 0x62, 0xB3, 0x57,
        0x16, 0x39, 0xD6, 0x24, 0xE6, 0x51, 0x52, 0xAB,
        0x8F, 0x53, 0x0C, 0x35, 0x9F, 0x08, 0x61, 0xD8,
        0x07, 0xCA, 0x0D, 0xBF, 0x50, 0x0D, 0x6A, 0x61,
        0x56, 0xA3, 0x8E, 0x08, 0x8A, 0x22, 0xB6, 0x5E,
        0x52, 0xBC, 0x51, 0x4D, 0x16, 0xCC, 0xF8, 0x06,
        0x81, 0x8C, 0xE9, 0x1A, 0xB7, 0x79, 0x37, 0x36,
        0x5A, 0xF9, 0x0B, 0xBF, 0x74, 0xA3, 0x5B, 0xE6,
        0xB4, 0x0B, 0x8E, 0xED, 0xF2, 0x78, 0x5E, 0x42,
        0x87, 0x4D
    };

    int i, j;
    uint8_t result[114];

    chacha20_encrypt(
        key,
        nonce,
        counter,
        plaintext,
        result,
        length
    );

    printf("--- TESTING CHACHA20 ENCRYPT FUNCTION ---\n\n");
    for (i = 0; i < 6; ++i) {
        printf("R[%03d..%03d]: ", i * 19, i * 19 + 18);
        for (j = 0; j < 19; ++j) {
            printf("%02" PRIX8 " ", result[i * 19 + j]);
        }
        printf("\nE[%03d..%03d]: ", i * 19, i * 19 + 18);
        for (j = 0; j < 19; ++j) {
            printf("%02" PRIX8 " ", expected[i * 19 + j]);
        }
        printf("\n             ");
        for (j = 0; j < 19; ++j) {
            if (result[i * 19 + j] != expected[i * 19 + j]) {
                printf("^^ ");
            } else {
                printf("   ");
            }
        }
        putc('\n', stdout);
    }

    printf("Plaintext: %s\n", plaintext);
    chacha20_encrypt(
        key,
        nonce,
        counter,
        result,
        result,
        length
    );
    printf("Decrypted: %s\n", (char *) result);
}

int main(void) {
    test_chacha20_block();
    test_chacha20_encrypt();
    return 0;
}
