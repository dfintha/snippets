/* uuid.c
 * Basic UUID generation (version 4) and formatting implemented in ANSI C.
 * Author: Dénes Fintha
 * Year: 2023
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t time_high;
    uint16_t time_low;
    uint16_t reserved;
    uint8_t family;
    uint8_t node[7];
} uuid_t;

typedef enum {
    UUID_FORMAT_UPPERCASE = 0x01,
    UUID_FORMAT_OMIT_DASHES = 0x02,
    UUID_FORMAT_ADD_BRACES = 0x04,
    UUID_FORMAT_ADD_NEWLINE = 0x08
} uuid_format_t;

extern const uuid_t uuid_nil;
extern const uuid_t uuid_max;

uuid_t uuid_generate_v4(void);
void uuid_fprint(uuid_t uuid, uuid_format_t format_bits, FILE *stream);
void uuid_sprint(uuid_t uuid, uuid_format_t format_bits, char *buffer);
void uuid_print(uuid_t uuid, uuid_format_t format_bits);
int uuid_compare(uuid_t lhs, uuid_t rhs);

/* Implementation */

#include <stdlib.h>
#include <string.h>

const uuid_t uuid_nil = {
    0x00000000,
    0x0000,
    0x0000,
    0x00,
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const uuid_t uuid_max = {
    0xFFFFFFFF,
    0xFFFF,
    0xFFFF,
    0xFF,
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

static uint8_t generate_random_byte(void) {
    /* More sophisticated PRNGs could be used, like MT19937 or ChaCha20. */
    return rand() & 0xFF;
}

uuid_t uuid_generate_v4(void) {
    union {
        uuid_t uuid;
        uint8_t bytes[16];
    } result;
    uint8_t i;
    for (i = 0; i < 16; ++i) {
        result.bytes[i] = generate_random_byte();
    }
    result.bytes[6] &= 0x0F;
    result.bytes[6] |= 0x40;
    result.bytes[8] &= 0x3F;
    result.bytes[8] |= 0x80;
    return result.uuid;
}

void uuid_fprint(uuid_t uuid, uuid_format_t format_bits, FILE *stream) {
    char buffer[64];
    uuid_sprint(uuid, format_bits, buffer);
    fputs(buffer, stream);
}

void uuid_sprint(uuid_t uuid, uuid_format_t format_bits, char *buffer) {
    static const char * const format_uppercase =
        "%s"
        "%02X%02X%02X%02X%s"
        "%02X%02X%s"
        "%02X%02X%s"
        "%02X%02X%s"
        "%02X%02X%02X%02X%02X%02X"
        "%s%s";
    static const char * const format_lowercase =
        "%s"
        "%02x%02x%02x%02x%s"
        "%02x%02x%s"
        "%02x%02x%s"
        "%02x%02x%s"
        "%02x%02x%02x%02x%02x%02x"
        "%s%s";
    const uint8_t *bytes = (const uint8_t *) &uuid;
    const int uppercase = (format_bits & UUID_FORMAT_UPPERCASE) != 0;
    const int omit_dashes = (format_bits & UUID_FORMAT_OMIT_DASHES) != 0;
    const int add_braces = (format_bits & UUID_FORMAT_ADD_BRACES) != 0;
    const int add_newline = (format_bits & UUID_FORMAT_ADD_NEWLINE) != 0;
    sprintf(
        buffer,
        uppercase ? format_uppercase : format_lowercase,
        add_braces ? "{" : "",
        bytes[0], bytes[1], bytes[2], bytes[3],
        omit_dashes ? "" : "-",
        bytes[4], bytes[5],
        omit_dashes ? "" : "-",
        bytes[6], bytes[7],
        omit_dashes ? "" : "-",
        bytes[8], bytes[9],
        omit_dashes ? "" : "-",
        bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15],
        add_braces ? "}" : "",
        add_newline ? "\n" : ""
    );
}

void uuid_print(uuid_t uuid, uuid_format_t format_bits) {
    uuid_fprint(uuid, format_bits, stdout);
}

int uuid_compare(uuid_t lhs, uuid_t rhs) {
    return memcmp(&lhs, &rhs, sizeof(uuid_t));
}

/* Demonstration */

#include <time.h>

int main(void) {
    uuid_t uuid;
    uint8_t i;
    const uuid_format_t format_bits =
        UUID_FORMAT_UPPERCASE |
        UUID_FORMAT_ADD_BRACES |
        UUID_FORMAT_ADD_NEWLINE;

    printf("       Nil: ");
    uuid_print(uuid_nil, format_bits);
    printf("       Max: ");
    uuid_print(uuid_max, format_bits);

    srand(time(NULL));
    for (i = 0; i < 10; ++i) {
        uuid = uuid_generate_v4();
        printf(" Generated: ");
        uuid_print(uuid, format_bits);
    }

    return 0;
}
