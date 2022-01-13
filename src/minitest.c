// minitest.c
// A minimal, single-header test suite, along with demonstration code for its
// usage. This code requires at least C99 or C++98 to run.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#if !defined(MINITEST_HEADER_VERSION)
#define MINITEST_HEADER_VERSION 1

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef bool(*minitest_test_case)(void *user_data);
typedef void(*minitest_setup_function)(void *user_data);
typedef void(*minitest_teardown_function)(void *user_data);

#define minitest_str(s) #s
#define minitest_xstr(s) minitest_str(s)

static void * const minitest_no_user_data = NULL;
static minitest_setup_function const minitest_no_setup_fn = NULL;
static minitest_teardown_function const minitest_no_teardown_fn = NULL;

#define minitest_assert_internal(criteria, file, line, fn, msg, ...)        \
    do {                                                                    \
        if (!(criteria)) {                                                  \
            printf("failed\n");                                             \
            printf("    error: ");                                          \
            printf(msg, __VA_ARGS__);                                       \
            printf("\n           at %s:%d", file, line);                    \
            printf("\n           in %s\n", fn);                             \
            return false;                                                   \
        }                                                                   \
    } while (false)

#define minitest_assert_unreachable()                                       \
    minitest_assert_internal(                                               \
        false,                                                              \
        __FILE__,                                                           \
        __LINE__,                                                           \
        __func__,                                                           \
        "%s",                                                               \
        "unreachable code reached"                                          \
    )

#define minitest_assert_intcompare(int1, rel, int2)                         \
    minitest_assert_internal(                                               \
        int1 rel int2,                                                      \
        __FILE__,                                                           \
        __LINE__,                                                           \
        __func__,                                                           \
        "integer comparison failed (%d " minitest_xstr(rel) " %d)",         \
        int1, int2                                                          \
    )

#define minitest_assert_fltcompare(flt1, rel, flt2, eps)                    \
    minitest_assert_internal(                                               \
        (fabs(flt1 - flt2) < eps) rel true,                                 \
        __FILE__,                                                           \
        __LINE__,                                                           \
        __func__,                                                           \
        "floating-point comparison failed (%f " minitest_xstr(rel) " %f)",  \
        flt1, flt2                                                          \
    )

#define minitest_assert_strcompare(str1, rel, str2)                         \
    minitest_assert_internal(                                               \
        strcmp(str1, str2) rel 0,                                           \
        __FILE__,                                                           \
        __LINE__,                                                           \
        __func__,                                                           \
        "string comparison failed (\"%s\" " minitest_xstr(rel) " \"%s\")",  \
        str1, str2                                                          \
    )

#define minitest_assert_objcompare(obj1, rel, obj2, cmp)                    \
    minitest_assert_internal(                                               \
        cmp(obj1, obj2) rel true,                                           \
        __FILE__,                                                           \
        __LINE__,                                                           \
        __func__,                                                           \
        "%s",                                                               \
        "object comparison failed"                                          \
    )

#define minitest_assert_that(expr)                                          \
    minitest_assert_internal(                                               \
        expr,                                                               \
        __FILE__,                                                           \
        __LINE__,                                                           \
        __func__,                                                           \
        "%s",                                                               \
        "assertion failed (" minitest_xstr(expr) ")"                        \
    )

#define minitest_test_case(name)                                            \
    minitest_xstr(name), name

static void
minitest_run_test_group(
    const char *test_group_name,
    minitest_setup_function setup,
    minitest_teardown_function teardown,
    void *user_data,
    ...
)
{
    va_list list;
    va_start(list, user_data);

    const char *current_name = NULL;
    minitest_test_case current_test = NULL;

    printf("\nrunning test group '%s'\n", test_group_name);

    size_t counter = 0;
    size_t failed_counter = 0;

    while (true) {
        if (counter % 2 == 0) {
            current_name = va_arg(list, const char *);
            if (current_name == NULL)
                break;
        } else {
            current_test = va_arg(list, minitest_test_case);
            if (current_test == NULL)
                break;

            printf("  %s/%s: ", test_group_name, current_name);

            if (setup != NULL)
                setup(user_data);

            const bool result = current_test(user_data);

            if (teardown != NULL)
                teardown(user_data);

            if (result) {
                printf("passed\n");
            } else {
                ++failed_counter;
            }
        }

        ++counter;
    }

    va_end(list);

    if (failed_counter == 0) {
        printf("all tests passed\n\n");
    } else {
        printf("%ld of %ld tests failed\n\n", failed_counter, counter / 2);
    }
}

#endif /* MINITEST_HEADER_VERSION */

// Demonstration

typedef struct {
    int a;
    double b;
} fixture_t;

void fixture_setup(void *user_data) {
    fixture_t *fixture = (fixture_t *) user_data;
    fixture->a = 42;
    fixture->b = 3.14;
}

void fixture_teardown(void *user_data) {
    (void)(user_data);
}

bool fixture_compare(fixture_t *lhs, fixture_t *rhs) {
    return (lhs->a == rhs->a) && ((lhs->b - rhs->b) < DBL_EPSILON);
}

bool check_setup_and_object_comparison(void *user_data) {
    fixture_t reference = { .a = 42, .b = 3.14 };
    fixture_t *fixture = (fixture_t *) user_data;
    minitest_assert_objcompare(fixture, ==, &reference, fixture_compare);
    return true;
    minitest_assert_unreachable();
}

bool check_primitive_comparison(void *user_data) {
    (void)(user_data);
    minitest_assert_intcompare(1, ==, 1);
    minitest_assert_fltcompare(3.14, ==, 3.14, FLT_EPSILON);
    minitest_assert_strcompare("testing", ==, "testing");
    return true;
}

bool check_test_failure_with_custom_assertion(void *user_data) {
    (void)(user_data);
    minitest_assert_that((1 > 3.14 && "test") || false);
}

int main(void) {
    fixture_t fixture;
    minitest_run_test_group(
        "minitest_test_with_fixture",
        fixture_setup,
        fixture_teardown,
        &fixture,
        minitest_test_case(check_setup_and_object_comparison),
        minitest_test_case(check_setup_and_object_comparison),
        NULL
    );
    minitest_run_test_group(
        "minitest_test_without_fixture",
        minitest_no_setup_fn,
        minitest_no_teardown_fn,
        minitest_no_user_data,
        minitest_test_case(check_primitive_comparison),
        minitest_test_case(check_test_failure_with_custom_assertion),
        NULL
    );

    return 0;
}
