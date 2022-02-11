// c_utilities.c
// Small utility functions that may prove useful in C.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define MACRO_SCOPE_BEGIN   do {
#define MACRO_SCOPE_END     } while (0)
#define MACRO_CONCAT(x1, x2) x1##x2
#define MACRO_XCONCAT_2(x1, x2) MACRO_CONCAT(x1, x2)
#define MACRO_XCONCAT_3(x1, x2, x3) MACRO_XCONCAT_2(x1, MACRO_XCONCAT_2(x2, x3))
#define MACRO_XCONCAT_4(x1, x2, x3, x4) MACRO_XCONCAT_2(x1, MACRO_XCONCAT_3(x2, x3, x4))
#define MACRO_STRINGIFY(x) #x
#define MACRO_XSTRINGIFY(x) MACRO_STRINGIFY(x)

#if defined(__GNUC__)
#define UNUSED(...) MACRO_XCONCAT_3(__VA_ARGS__, _unused_line, __LINE__) __attribute__((unused))
#else
#define UNUSED(...) MACRO_XCONCAT_3(__VA_ARGS__, _unused_line, __LINE__)
#endif

#define primitive_swap(a, b, type)              \
    MACRO_SCOPE_BEGIN                           \
        type swap##_##a##_##b##_##type = a;     \
        a = b;                                  \
        b = swap##_##a##_##b##_##type;          \
    MACRO_SCOPE_END

typedef int(*char_transformation_fn)(int);
typedef int(*char_predicate_fn)(int);

const char * boolyn(bool value);
const char * booltf(bool value);
float decroundf(float x, int decimals);
double decround(double x, int decimals);
long double decroundl(long double x, int decimals);
void memclr(void *address, size_t length);
void memswap(void *first, void *second, size_t length);
void shuffle(void *array, size_t size, size_t count);
char * sizefmt(size_t size);
bool streq(const char *str1, const char *str2);
ptrdiff_t strfindc(const char *haystack, int needle);
ptrdiff_t strfinds(const char *haystack, const char *needle);
char * strtransform(char *string, char_transformation_fn transformation);
char * strtrim(char *string, char_predicate_fn predicate);
size_t strfirst(const char *string, char_predicate_fn predicate);
size_t strfirstnot(const char *string, char_predicate_fn predicate);
size_t strlast(const char *string, char_predicate_fn predicate);
size_t strlastnot(const char *string, char_predicate_fn predicate);
int chany(char character, const char *set);
void fprintnl(FILE *stream);
void printnl(void);

// Implementation

#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char * boolyn(bool value) {
    return value ? "yes" : "no";
}

const char * booltf(bool value) {
    return value ? "true" : "false";
}

float decroundf(float x, int decimals) {
  const float multiplier = (float)(pow(10, decimals));
  return roundf(x * multiplier) / multiplier;
}

double decround(double x, int decimals) {
  const double multiplier = pow(10, decimals);
  return round(x * multiplier) / multiplier;
}

long double decroundl(long double x, int decimals) {
  const double multiplier = (long double)(pow(10, decimals));
  return roundl(x * multiplier) / multiplier;
}

void memclr(void *address, size_t length) {
    memset(address, 0x00, length);
}

void memswap(void *first, void *second, size_t length) {
    void *temp = malloc(length);
    memmove(temp, second, length);
    memmove(second, first, length);
    memmove(first, temp, length);
    free(temp);
}

void shuffle(void *array, size_t size, size_t count) {
    srand(time(NULL));
    char *bytes = (char *) array;
    for (size_t i = 0; i < count ; ++i) {
        const size_t j = rand() % count;
        memswap(bytes + i * size, bytes + j * size, size);
    }
}

char * sizefmt(size_t size) {
    static const char prefixes[] = " KMGTPEZY";
    static const int max = sizeof(prefixes) - 1;
    static const int step = 1024;
    char *buffer = (char *) malloc(16);

    double work = (double) size;
    short index = 0;
    while (work >= step && index < max) {
        work /= step;
        ++index;
    }

    static const char* with_prefix = "%.2f %ciB";
    static const char* without_prefix = "%.2f B";
    const char *format = (index > 0) ? with_prefix : without_prefix;
    snprintf(buffer, 15, format, work, prefixes[index]);

    char* decimal = strstr(buffer, ".");
    if (decimal != NULL) {
        setlocale(LC_NUMERIC, "");
        struct lconv* lc = localeconv();
        *decimal = *lc->decimal_point;
    }

    return buffer;
}

bool streq(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

ptrdiff_t strfindc(const char *haystack, int needle) {
    const char *where = strchr(haystack, needle);
    if (where == NULL)
        return -1;
    return where - haystack;
}

ptrdiff_t strfinds(const char *haystack, const char *needle) {
    const char *where = strstr(haystack, needle);
    if (where == NULL)
        return -1;
    return where - haystack;
}

char * strtransform(char *string, char_transformation_fn transformation) {
    const size_t length = strlen(string);
    for (size_t i = 0; i < length; ++i) {
        string[i] = transformation(string[i]);
    }
    return string;
}

char * strtrim(char *string, char_predicate_fn predicate) {
    const size_t first = strfirstnot(string, predicate);
    const size_t last = strlastnot(string, predicate);
    const size_t length = last - first + 1;
    strncpy(string, string + first, length);
    string[length] = '\0';
    return string;
}

size_t strfirst(const char *string, char_predicate_fn predicate) {
    const size_t length = strlen(string);
    size_t where;

    for (where = 0; where < length; ++where) {
        if (predicate(string[where]))
            break;
    }

    return where;
}

size_t strfirstnot(const char *string, char_predicate_fn predicate) {
    const size_t length = strlen(string);
    size_t where;

    for (where = 0; where < length; ++where) {
        if (!predicate(string[where]))
            break;
    }

    return where;
}

size_t strlast(const char *string, char_predicate_fn predicate) {
    const size_t length = strlen(string);
    size_t where;

    for (where = 0; where < length; ++where) {
        if (predicate(string[length - where - 1]))
            break;
    }

    return length - where - 1;
}

size_t strlastnot(const char *string, char_predicate_fn predicate) {
    const size_t length = strlen(string);
    size_t where;

    for (where = 0; where < length; ++where) {
        if (!predicate(string[length - where - 1]))
            break;
    }

    return length - where - 1;
}

int chany(char character, const char *set) {
    const size_t length = strlen(set);
    for (size_t i = 0; i < length; ++i) {
        if (character == set[i])
            return 1;
    }
    return 0;
}

void fprintnl(FILE *stream) {
    fputc('\n', stream);
}

void printnl(void) {
    fprintnl(stdout);
}

// Demonstration

#include <ctype.h>

static int is_basic_whitespace(int character) {
    return chany(character, " \t\n");
}

static void print_array(int *array, int length) {
    fputs("{ ", stdout);
    for (int i = 0; i < length; ++i) {
        printf("%d ", array[i]);
    }
    putc('}', stdout);
}

int main(UNUSED(int argc), UNUSED(char **argv)) {
    puts("BOOLEAN TO STRING FORMATTING");
    puts("value | boolyn(value) | booltf(value)");
    puts("------|---------------|--------------");
    printf("%-5d | %-13s | %-13s\n", true, boolyn(true), booltf(true));
    printf("%-5d | %-13s | %-13s\n", false, boolyn(false), booltf(false));
    printnl();

    puts("ROUNDING FLOATS TO N DECIMALS");
    puts("value     | type of value | decround*(value, 2)");
    puts("----------|---------------|--------------------");
    printf("%-9f | %-13s | %-12f\n", 3.1415926535F, "float", decroundf(3.1415926535F, 2));
    printf("%-9lf | %-13s | %-12lf\n", 3.1415926535, "double", decround(3.1415926535, 2));
    printf("%-9Lf | %-13s | %-12Lf\n", 3.1415926535L, "long double", decroundl(3.1415926535L, 2));
    printnl();

    char buffer1[10] = "ABCDEFGHI";
    char buffer2[10] = "JKLMNOPQR";
    puts("MEMORY CHUNK SWAPPING");
    puts("buffer1    | buffer2    | state");
    puts("-----------|------------|------------");
    printf("%-10s | %-10s | before swap\n", buffer1, buffer2);
    memswap(buffer1, buffer2, sizeof(buffer1));
    printf("%-10s | %-10s | after swap\n", buffer1, buffer2);
    printnl();

    puts("SUBSTRING AND CHARACTER LOOKUP");
    puts("buffer     | substring or character | result");
    puts("-----------|------------------------|-------");
    printf("%-10s | %-22s | %-6td\n", buffer1, "\"MNO\"", strfinds(buffer1, "MNO"));
    printf("%-10s | %-22s | %-6td\n", buffer1, "\"ABC\"", strfinds(buffer1, "ABC"));
    printf("%-10s | %-22s | %-6td\n", buffer1, "'A'", strfindc(buffer1, 'A'));
    printf("%-10s | %-22s | %-6td\n", buffer1, "'M'", strfindc(buffer1, 'M'));
    printnl();

    puts("STRING EQUALITY CHECK");
    puts("buffer1    | buffer2    | streq(buffer1, buffer2)");
    puts("-----------|------------|------------------------");
    printf("%-10s | %-10s | %s\n", buffer1, buffer2, booltf(streq(buffer1, buffer2)));
    printf("%-10s | %-10s | %s\n", buffer1, buffer1, booltf(streq(buffer1, buffer1)));
    printnl();

    puts("MEMORY CLEARING");
    puts("buffer     | strlen(buffer) | state");
    puts("-----------|----------------|-------------");
    printf("%-10s | %-14ld | before clear\n", buffer1, strlen(buffer1));
    memclr(buffer1, sizeof(buffer1));
    printf("%-10s | %-14ld | after clear\n", buffer1, strlen(buffer1));
    printnl();

    puts("DATA SIZE FORMATTING");
    puts("value                  | sizefmt(value)");
    puts("-----------------------|---------------");
    char *formatted = sizefmt(2147483648);
    printf("%-22lu | %-14s\n", 2147483648, formatted);
    free(formatted);
    formatted = sizefmt(1024);
    printf("%-22lu | %-14s\n", 1024UL, formatted);
    free(formatted);
    formatted = sizefmt(467445623434543);
    printf("%-22lu | %-14s\n", 467445623434543, formatted);
    free(formatted);
    printnl();

    int array[] = { 1, 2, 3, 4, 5 };
    puts("ARRAY SHUFFLING");
    puts("array         | state");
    puts("--------------|---------------");
    print_array(array, sizeof(array) / sizeof(int));
    puts(" | before shuffle");
    shuffle(array, sizeof(int), 5);
    print_array(array, sizeof(array) / sizeof(int));
    puts(" | after shuffle");
    printnl();

    char hello_str[] = "   hello   ";
    puts("STRING MANIPULATION");
    puts("string      | state");
    puts("------------|----------------------------------");
    printf("%-11s | original\n", hello_str);
    printf("%-11s | after trim with whitespace filter\n", strtrim(hello_str, is_basic_whitespace));
    printf("%-11s | after transform with toupper\n", strtransform(hello_str, toupper));
    printnl();

    int value1 = 10;
    int value2 = 20;
    puts("PRIMITIVE SWAPPING");
    puts("value1 | value2 | state");
    puts("-------|--------|------------");
    printf("%-6d | %-6d | before swap\n", value1, value2);
    primitive_swap(value1, value2, int);
    printf("%-6d | %-6d | after swap\n", value1, value2);
    printnl();

#define X 42
    puts("STRINGIFICATION");
    puts("X  | MACRO_STRINGIFY(X) | MACRO_XSTRINGIFY(X)");
    puts("---|--------------------|--------------------");
    printf("%-2d | %-18s | %-19s\n", X, MACRO_STRINGIFY(X), MACRO_XSTRINGIFY(X));
    printnl();
#undef X

    puts("NOTES");
    puts(" 1. The UNUSED macro is demonstrated by the compilation yielding");
    puts("    no warnings for unused variables.");
    puts(" 2. Functions not explicitly tested are either re-used by another");
    puts("    function, thus proving they work, or are way too simple or");
    puts("    similar to other function(s), that demonstrating them is not");
    puts("    necessary.");

    return EXIT_SUCCESS;
}
