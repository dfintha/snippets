// absolute.cpp
// Absolute value calculation using bit flipping on IEEE 754 binary
// floating-point numbers.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <type_traits>

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type absolute(T x) {
  (reinterpret_cast<unsigned char *>(&x)[sizeof(T) - 1]) &= 0x7F;
  return x;
}

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type absolute(T x) {
  return (x < 0) ? -x : x;
}

// Demonstration

#include <cstdio>
#include <cstdlib>

int main() {
    printf("%.02f\n", absolute(-3.13F));
    printf("%.02f\n", absolute(+3.13F));
    printf("%.02f\n", absolute(-3.13));
    printf("%.02f\n", absolute(+3.13));
    printf("%d\n", absolute(-3));
    printf("%d\n", absolute(+3));
    return EXIT_SUCCESS;
}
