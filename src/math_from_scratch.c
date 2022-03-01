// math_from_scratch.c
// Basic mathematical functions written in plain C from scratch.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <stdbool.h>

#define PI 3.14159265359

bool near(double x, double y);
double power(double x, unsigned n);
double sine(double x);
double cosine(double x);
double tangent(double x);
double cotangent(double x);
double secant(double x);
double cosecant(double x);

// Implementation

#include <float.h>

bool near(double x, double y) {
    const double difference = (x - y);
    const double absolute = (difference < 0) ? (difference * -1) : (difference);
    return absolute <= DBL_EPSILON;
}

double power(double x, unsigned n) {
    if (n == 0)
        return 1.0;
    if (n % 2 == 0)
        return power(x * x, n / 2);
    return x * power(x, n - 1);
}

double factorial(unsigned n) {
    if (n <= 1)
        return 1.0;
    return n * factorial(n - 1);
}

double sine(double x) {
    double result = 0.0;
    for (unsigned n = 0; n < 100; ++n) {
        const double sign = (n % 2 == 0) ? 1 : -1;
        result += (sign * power(x, 2 * n + 1)) / factorial(2 * n + 1);
    }
    return result;
}

double cosine(double x) {
    double result = 0.0;
    for (unsigned n = 0; n < 100; ++n) {
        const double sign = (n % 2 == 0) ? 1 : -1;
        result += (sign * power(x, 2 * n)) / factorial(2 * n);
    }
    return result;
}

double tangent(double x) {
    double numerator = 0.0;
    double denumerator = 0.0;
    for (unsigned n = 0; n < 100; ++n) {
        const double sign = (n % 2 == 0) ? 1 : -1;
        numerator += (sign * power(x, 2 * n + 1)) / factorial(2 * n + 1);
        denumerator += (sign * power(x, 2 * n)) / factorial(2 * n);
    }
    return numerator / denumerator;
}

double cotangent(double x) {
    return 1.0 / tangent(x);
}

double secant(double x) {
    return 1.0 / cosine(x);
}

double cosecant(double x) {
    return 1.0 / sine(x);
}

// Demonstration

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(void) {
    srand(time(NULL));

    const double a = rand() % 9 + 1;
    const double b = rand() % 9 + 1;
    const double x = a + b / 10;
    const unsigned n = rand() % 4 + 2;

    printf("x = %.1lf\n", x);
    printf("n = %u\n", n);
    printf("x^n == %lf == %lf\n", power(x, n), pow(x, n));
    printf("n! == %.0lf\n", factorial(n));
    printf("sin(x) == %lf == %lf\n", sine(x), sin(x));
    printf("cos(x) == %lf == %lf\n", cosine(x), cos(x));
    printf("tan(x) == %lf == %lf\n", tangent(x), tan(x));

    return EXIT_SUCCESS;
}
