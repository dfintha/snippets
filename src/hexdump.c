/* hexdump.c
 * Function to hexdump a contiguous memory chunk on a specified address. The
 * example displays a dump with the first 260 bytes of a PDF file.
 * Author: Dénes Fintha
 * Year: 2022
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>
#include <stdio.h>

void fhexdump(FILE *stream, const void *address, size_t size);
void hexdump(const void *address, size_t size);

/* Implementation */

#include <ctype.h>
#include <stdint.h>

static size_t hexlen(size_t number) {
    size_t digits = 1;
    while (number > 1) {
        number /= 16;
        digits += 1;
    }
    return digits;
}

void fhexdump(FILE *stream, const void *address, size_t size) {
    const uint8_t *bytes = (const uint8_t *) address;
    const size_t chunk_size = 16;
    const size_t chunks = size / chunk_size;

    size_t i, j;
    const uint8_t *base;
    size_t current_size;
    char format[8];

    sprintf(format, "0x%%0%luX: ", hexlen(size));

    for (i = 0; i < chunks + 1; ++i) {
        current_size = (i == chunks) ? size % chunk_size : chunk_size;
        base = bytes + (i * chunk_size);

        fprintf(stream, format, i * chunk_size);
        for (j = 0; j < chunk_size; ++j) {
            if (j < current_size) {
                fprintf(stream, "%02X ", base[j]);
            } else {
                fputs("   ", stream);
            }
        }
        fputc(' ', stream);
        for (j = 0; j < current_size; ++j) {
            fputc(isprint(base[j]) ? base[j] : '.', stream);
        }
        fputc('\n', stream);
    }
}

void hexdump(const void *address, size_t size) {
    fhexdump(stdout, address, size);
}

/* Demonstration */

int main(void) {
    char pdf_file_start[] =
        "\x25\x50\x44\x46\x2d\x31\x2e\x34\x0a\x25\xc7\xec\x8f\xa2\x0a\x25\x25"
        "\x49\x6e\x76\x6f\x63\x61\x74\x69\x6f\x6e\x3a\x20\x67\x73\x20\x2d\x73"
        "\x44\x45\x56\x49\x43\x45\x3d\x70\x64\x66\x77\x72\x69\x74\x65\x20\x2d"
        "\x64\x43\x6f\x6d\x70\x61\x74\x69\x62\x69\x6c\x69\x74\x79\x4c\x65\x76"
        "\x65\x6c\x3d\x31\x2e\x34\x20\x2d\x64\x4e\x4f\x50\x41\x55\x53\x45\x20"
        "\x2d\x64\x51\x55\x49\x45\x54\x20\x2d\x64\x42\x41\x54\x43\x48\x20\x2d"
        "\x73\x4f\x75\x74\x70\x75\x74\x46\x69\x6c\x65\x3d\x3f\x20\x3f\x0a\x35"
        "\x20\x30\x20\x6f\x62\x6a\x0a\x3c\x3c\x2f\x4c\x65\x6e\x67\x74\x68\x20"
        "\x36\x20\x30\x20\x52\x2f\x46\x69\x6c\x74\x65\x72\x20\x2f\x46\x6c\x61"
        "\x74\x65\x44\x65\x63\x6f\x64\x65\x3e\x3e\x0a\x73\x74\x72\x65\x61\x6d"
        "\x0a\x78\x9c\xe5\x5a\xd9\x6e\x1c\xc7\x15\xad\xc0\x0f\x06\x26\x81\x16"
        "\x58\xca\x62\xe7\xa1\x81\xd8\x52\x8f\x9d\x69\xd7\xbe\xf8\x31\x18\x91"
        "\x94\xc2\x98\x22\xc5\xcd\x12\xf3\x10\x30\x1b\x02\x31\x89\x0c\x04\xf9"
        "\x34\x7f\x4a\x3e\x21\xbf\x91\x73\x6f\x55\xf5\x74\xcf\x0c\x47\xd2\x30"
        "\xb2\x10\x04\x04\xc1\xa9\xea\xee\xba\xfb\x39\xf7\xf6\xf0\x55\x23\x3b"
        "\xd5\x48\xfa\x29";
    hexdump(pdf_file_start, sizeof(pdf_file_start));
    return 0;
}
