// number_utilities.cpp
// Collector snippet for various number utility function templates. Compatible
// with C++98.
// Author: Dénes Fintha
// Year: 2024
// -------------------------------------------------------------------------- //

// Interface & Implementation

#include <set>
#include <vector>

template <typename IntT>
IntT largest_prime_palindrome_under(IntT limit) {
    std::vector<IntT> cache;
    cache.push_back(2);

    for (IntT i = 3; i < limit; ++i) {
        typename std::vector<IntT>::iterator it = cache.begin();
        while (it != cache.end()) {
            if (i % *it++ == 0)
                break;
        }
        if (it == cache.end())
            cache.push_back(i);
    }

    typename std::vector<IntT>::reverse_iterator it;
    for (it = cache.rbegin(); it != cache.rend(); ++it) {
        std::vector<int> digits;
        IntT number = *it;
        while (number != 0) {
            digits.push_back(number % 10);
            number /= 10;
        }

        std::vector<int>::iterator fit = digits.begin();
        std::vector<int>::reverse_iterator bit = digits.rbegin();
        for (std::vector<int>::size_type i = 0; i < digits.size(); ++i) {
            if (*fit++ != *bit++)
                break;
        }

        if (fit == digits.end())
            return *it;
    }

    return static_cast<IntT>(0);
}

template <typename IntT>
bool is_happy_number(IntT number) {
    std::set<IntT> visited;
    do {
        visited.insert(number);
        IntT step = 0;
        while (number != 0) {
            const IntT digit = number % 10;
            step += digit * digit;
            number = number / 10;
        }
        number = step;
    } while (number != 1 && visited.count(number) != 1);
    return number == 1;
}

template <typename IntT>
static void calculate_primes_up_to(std::vector<IntT>& primes, IntT number) {
    const IntT current_maximum = primes.back();
    if (current_maximum >= number)
        return;

    for (IntT i = current_maximum; i <= number; ++i) {
        typename std::vector<IntT>::iterator it = primes.begin();
        while (it != primes.end()) {
            if (i % *it++ == 0)
                break;
        }
        if (it == primes.end())
            primes.push_back(i);
    }
}

template <typename IntT>
const std::vector<IntT>& cache_primes_up_to(IntT number) {
    static std::vector<IntT> primes(1, 2);
    calculate_primes_up_to(primes, number);
    return primes;
}

template <typename IntT>
std::vector<IntT> cache_primes_up_to_cloned(IntT number) {
    std::vector<IntT> primes(1, 2);
    calculate_primes_up_to(primes, number);
    return primes;
}

template <typename IntT>
std::vector<IntT> factorize(IntT number, bool multithreaded = false) {
    const std::vector<IntT>& primes = multithreaded
        ? cache_primes_up_to_cloned(number)
        : cache_primes_up_to(number);

    std::vector<IntT> factors;
    IntT index = 0;
    while (number != 1) {
        while (number % primes[index] != 0)
            ++index;
        factors.push_back(primes[index]);
        number /= primes[index];
    }
    return factors;
}

// Demonstration

#include <cstdio>
#include <cstdlib>
#include <ctime>

static const std::set<int>& get_happy_number_reference() {
    static std::set<int> reference;
    static bool initialized = false;
    if (!initialized) {
        reference.insert(  1);
        reference.insert(  7);
        reference.insert( 10);
        reference.insert( 13);
        reference.insert( 19);
        reference.insert( 23);
        reference.insert( 28);
        reference.insert( 31);
        reference.insert( 32);
        reference.insert( 44);
        reference.insert( 49);
        reference.insert( 68);
        reference.insert( 70);
        reference.insert( 79);
        reference.insert( 82);
        reference.insert( 86);
        reference.insert( 91);
        reference.insert( 94);
        reference.insert( 97);
        reference.insert(100);
        initialized = true;
    }
    return reference;
}

static void prime_palindrome_test(int top, int expected) {
    const int found = largest_prime_palindrome_under<int>(top);
    const bool same = (expected == found);
    printf(
        "  Largest prime palindrome under %d should be %d, found %d. %s.\n",
        top,
        expected,
        found,
        same ? "Passed" : "Failed"
    );
}

static void happy_number_test(int to_check) {
    const bool result = is_happy_number<int>(to_check);
    const bool expected = get_happy_number_reference().count(to_check) == 1;
    const bool same = (result == expected);
    printf(
        "  Number %d %s a happy number, calculated it %s one. %s.\n",
        to_check,
        expected ? "is" : "is not",
        result ? "to be" : "not to be",
        same ? "Passed" : "Failed"
    );
}

static void factorization_test() {
    static const int min = 10000;
    static const int max = 99999;
    const int number = rand() % (max - min + 1) + min;
    const std::vector<int> factors = factorize(number);
    printf("  The prime factors of %d are calculated to be { ", number);
    printf("%d", factors[0]);
    if (factors.size() == 1)
        fputc(' ', stdout);
    for (size_t i = 1; i < factors.size(); ++i) {
        printf(", %d ", factors[i]);
    }
    puts("}");
}

int main() {
    srand(time(NULL));

    printf("\nPrime Palindrome Tests\n");
    prime_palindrome_test(100, 11);
    prime_palindrome_test(500, 383);
    prime_palindrome_test(1000, 929);
    prime_palindrome_test(10000, 929);
    prime_palindrome_test(15000, 14741);

    printf("\nHappy Number Tests\n");
    for (int i = 1; i < 100; i += 4) {
        happy_number_test(i);
    }

    printf("\nFactorization Tests\n");
    for (int i = 1; i < 10; ++i) {
        factorization_test();
    }

    puts("");
    return 0;
}
