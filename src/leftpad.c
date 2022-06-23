/* leftpad.c
 * The infamous leftpad function, rewritten in ANSI C.
 * Author: Dénes Fintha
 * Year: 2022
 * -------------------------------------------------------------------------- */

/* Interface */

#include <stddef.h>

char * leftpad(const char *str, long len, char fill);

/* Implementation */

#include <stdlib.h>
#include <string.h>

char * leftpad(const char *str, long len, char fill) {
    const long old_length = strlen(str);
    const long new_length = len <= old_length ? old_length : len;
    char *result = (char *) malloc(new_length + 1);
    const long pad_length = new_length - old_length;
    memset(result, fill, pad_length);
    strcpy(result + pad_length, str);
    return result;
}

/* Demonstration */

#include <stdio.h>

#define TEST(description, str, len, fill, expected) do {            \
    printf("Testing if " description "\n");                         \
    test = leftpad(str, len, fill);                                 \
    printf("  Expected: '%s'\n", expected);                         \
    printf("       Got: '%s'\n", test);                             \
    printf("    Result: %s\n\n",                                    \
           strcmp(expected, test) == 0 ? "Passed" : "Failed");      \
    free(test);                                                     \
} while (0)

int main(void) {
    char *test = NULL;
    TEST("padding an empty string to a length to 0 results in an empty string",
         "", 0, ' ', "");
    TEST("padding to a shorter length results in the same string",
         "foo", 2, ' ', "foo");
    TEST("padding to a negative length results in the same string",
         "foo", -2, ' ', "foo");
    TEST("padding a non-empty string to its length results in the same string",
         "foo", 3, ' ', "foo");
    TEST("padding to a longer string with a single character fills to the left",
         "foo", 4, '_', "_foo");
    return EXIT_SUCCESS;
}
