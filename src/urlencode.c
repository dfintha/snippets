// urlencode.c
// URL encoding and decoding functions. Results shall be freed by the caller.
// In C++, an std::string can be used to store the result of a conversion, and
// utilize RAII.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

char * urlencode(const char *string);
char * urldecode(const char *string);

// Implementation

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * urlencode(const char *string) {
    const unsigned char *bytes = (const unsigned char *) string;
    const size_t length = strlen(string);
    char *result = (char *) malloc(length * 3 + 1);
    for (size_t i = 0; i < length; ++i) {
        if (isalnum(bytes[i])) {
            strncat(result, string + i, 1);
        } else {
            char translated[4];
            snprintf(translated, 4, "%%" "%02X", bytes[i]);
            strcat(result, translated);
        }
    }
    return result;
}

static int is_hex_digit(char c) {
    c = toupper(c);
    return (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9');
}

char * urldecode(const char *string) {
    enum { NORMAL = 'N', PERCENT = 'P', BYTE = 'B', INVALID = 'I' } state = NORMAL;
    const size_t length = strlen(string);
    char *result = (char *) malloc(length + 1);
    size_t position = 0;
    char bytes[] = "0x00";
    for (size_t i = 0; i < length; ++i) {
        switch (state) {
            case NORMAL:
                if (string[i] == '%') {
                    state = PERCENT;
                } else {
                    result[position++] = string[i];
                }
                break;
            case PERCENT:
                if (!is_hex_digit(string[i])) {
                    state = INVALID;
                } else {
                    state = BYTE;
                    bytes[2] = toupper(string[i]);
                }
                break;
            case BYTE:
                if (!is_hex_digit(string[i])) {
                    state = INVALID;
                } else {
                    state = NORMAL;
                    bytes[3] = toupper(string[i]);
                    result[position++] = (char) strtol(bytes, NULL, 16);
                }
                break;
            case INVALID:
                free(result);
                return NULL;
        }
    }

    result[position] = '\0';
    return result;
}

// Demonstration

int main(int argc, char **argv) {
    const char *original = "Árvíztűrő Tükörfúrógép";
    char *encoded = urlencode(original);
    char *decoded = urldecode(encoded);
    printf("Original: %s\n", original);
    printf(" Encoded: %s\n", encoded);
    printf(" Decoded: %s\n", decoded);
    free(encoded);
    free(decoded);
    return 0;
}
