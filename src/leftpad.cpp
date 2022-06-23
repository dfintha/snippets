// leftpad.cpp
// The infamous leftpad function, rewritten in C++98.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>
#include <string>

std::string leftpad(const std::string& str, long len, char fill);
std::string leftpad(const std::string& str, long len, long fill);
std::string leftpad(const std::string& str, long len, const char *fill);

// Implementation

#include <cstdio>

std::string leftpad(const std::string& str, long len, char fill) {
    const long old_length = long(str.size());
    const long new_length = len <= old_length ? old_length : len;
    const long pad_length = new_length - old_length;
    std::string result = std::string(pad_length, fill);
    result += str;
    return result;
}

std::string leftpad(const std::string& str, long len, long fill) {
    char buffer[24];
    sprintf(buffer, "%ld", fill);
    return leftpad(str, len, buffer[0]);
}

std::string leftpad(const std::string& str, long len, const char *fill) {
    char f = (fill == NULL || fill[0] == '\0') ? ' ' : fill[0];
    return leftpad(str, len, f);
}

/* Demonstration */

#include <cstdio>

#define TEST(description, str, len, fill, expected) do {                   \
    printf("Testing if " description "\n");                                \
    std::string test = leftpad(str, len, fill);                            \
    printf("  Expected: '%s'\n", expected);                                \
    printf("       Got: '%s'\n", test.c_str());                            \
    printf("    Result: %s\n\n", test == expected ? "Passed" : "Failed");  \
} while (0)

int main(void) {
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
    TEST("padding to a longer string with a number fills with its first digit",
         "foo", 4, 12L, "1foo");
    TEST("padding to a longer string with a negative number fills with -",
         "foo", 4, -12L, "-foo");
    TEST("padding to a longer string with a string fills with its first char",
         "foo", 4, "abc", "afoo");
    return 0;
}
