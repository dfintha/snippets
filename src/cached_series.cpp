// cached_series.cpp
// Cached calculation of the fibonacci series and factorials.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>
#include <vector>

template <typename Integer>
Integer fibonacci(Integer index) {
    static std::vector<Integer> values{1, 1, 1};
    if (values.size() <= size_t(index))
        values.resize(index + 1, Integer(0));

    if (values[index] == Integer(0)) {
        values[index] = fibonacci(index - 1) + fibonacci(index - 2);
    }
    return values[index];
}

template <typename Integer>
Integer factorial(Integer index) {
    static std::vector<Integer> values{1, 1};
    if (values.size() <= size_t(index))
        values.resize(index + 1, Integer(0));

    if (values[index] == Integer(0))
        values[index] = index * factorial(index - 1);
    return values[index];
}

// Demonstration

#include <iostream>

int main() {
    std::cout << "(This calculation would take a while without caching.)" << std::endl;

    std::cout << "Calculating the first 90 fibonacci numbers." << std::endl;
    for (size_t i = 1; i <= 90; ++i) {
        std::cout << "fib(" << i << ") = " << fibonacci<uint64_t>(i) << std::endl;
    }

    std::cout << "Calculating the first 20 factorials." << std::endl;
    for (size_t i = 1; i <= 20; ++i) {
        std::cout << i << "! = " << factorial<uint64_t>(i) << std::endl;
    }

    return 0;
}
