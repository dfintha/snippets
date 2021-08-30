// congruent.cpp
// Simple congruency check with easy-to-read notation.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <cstdint>

class congruent {
public:
    congruent(uintmax_t a, uintmax_t b);
    bool modulo(uintmax_t m);
private:
    uintmax_t a;
    uintmax_t b;
};

// Implementation

congruent::congruent(uintmax_t a, uintmax_t b) : a(a), b(b) {
}

bool congruent::modulo(uintmax_t m) {
  return (a % m) == (b % m);
}

// Demonstration

#include <cstdio>
#include <cstdlib>

int main() {
    {
        const unsigned a = 13;
        const unsigned b = 23;
        const unsigned m = 10;
        printf("%d ≡ %d (mod %d) ? %s\n",
               a, b, m, congruent(a, b).modulo(m) ? "Yes" : "No");
    }
    {
        const unsigned a = 15;
        const unsigned b = 25;
        const unsigned m = 11;
        printf("%d ≡ %d (mod %d) ? %s\n",
               a, b, m, congruent(a, b).modulo(m) ? "Yes" : "No");
    }
    return EXIT_SUCCESS;
}
