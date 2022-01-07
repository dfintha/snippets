// c_utilities.c
// Small utility functions that may prove useful in C.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

// Implementation

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
        memswap(array + i * size, array + j * size, size);
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

// Demonstration

#include <stdio.h>

int main(void) {
    // boolean to string functions
    printf("%s %s %s %s\n", boolyn(true), boolyn(false), booltf(true), booltf(false));

    // rounding with all floating-point types
    printf("%f %lf %Lf\n", decroundf(3.1415F, 2), decround(3.1415, 2), decroundl(3.1415L, 2));

    char buffer1[10] = "ABCDEFGHI";
    char buffer2[10] = "JKLMNOPQR";

    // memory swapping
    printf("%s %s\n", buffer1, buffer2);
    memswap(buffer1, buffer2, sizeof(buffer1));
    printf("%s %s\n", buffer1, buffer2);

    // successful substring/character index lookup
    printf("%zd %zd\n", strfinds(buffer1, "MNO"), strfindc(buffer2, 'A'));

    // unsuccessful substring/character index lookup
    printf("%zd %zd\n", strfinds(buffer1, "ABC"), strfindc(buffer2, 'M'));

    // string equality check
    printf("%s %s\n", booltf(streq(buffer1, buffer1)), booltf(streq(buffer1, buffer2)));

    // a memory clearing shall make the buffer a zero-length string
    memclr(buffer1, sizeof(buffer1));
    printf("%lu\n", strlen(buffer1));

    // size formatting
    char *formatted = sizefmt(2147483648);
    printf("%s\n", formatted);
    free(formatted);

    // array shuffling
    int array[] = { 1, 2, 3, 4, 5 };
    printf("{ ");
    for (size_t i = 0; i < sizeof(array) / sizeof(int); ++i)
        printf("%d ", array[i]);
    printf("}\n{ ");
    shuffle(array, sizeof(int), 5);
    for (size_t i = 0; i < sizeof(array) / sizeof(int); ++i)
        printf("%d ", array[i]);
    printf("}\n");

    return EXIT_SUCCESS;
}
