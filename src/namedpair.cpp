// namedpair.cpp
// Macro defining named pair types based on std::pair for C++.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

#include <utility>

#define NAMEDPAIR(FIRST_T, FIRST_NAME, SECOND_T, SECOND_NAME)           \
    struct : public std::pair<FIRST_T, SECOND_T> {                      \
        using base = std::pair<FIRST_T, SECOND_T>;                      \
                                                                        \
        using std::pair<FIRST_T, SECOND_T>::pair;                       \
        using std::pair<FIRST_T, SECOND_T>::operator=;                  \
        using std::pair<FIRST_T, SECOND_T>::swap;                       \
                                                                        \
        base& as_pair() { return *this; }                               \
        const base& as_pair() const { return *this; }                   \
                                                                        \
        FIRST_T& FIRST_NAME() { return this->first; }                   \
        const FIRST_T& FIRST_NAME() const { return this->first; }       \
                                                                        \
        SECOND_T& SECOND_NAME() { return this->second; }                \
        const SECOND_T& SECOND_NAME() const { return this->second; }    \
    }

// Demonstration

#include <cstdio>
#include <string>

int main() {
    using status_t = NAMEDPAIR(int, code, std::string, message);
    const status_t result{0, "Success"};

    printf(
        "(%d, %s)\n",
        result.as_pair().first,
        result.as_pair().second.c_str()
    );

    printf(
        "(%d, %s)\n",
        result.code(),
        result.message().c_str()
    );

    return result.code();
}
