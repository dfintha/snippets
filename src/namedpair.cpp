// namedpair.cpp
// Macro defining named pair types based on std::pair for C++.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

#include <utility>

#define namedpair(first_type, first_name, second_type, second_name)     \
    struct : public std::pair<first_type, second_type> {                \
        using std::pair<first_type, second_type>::pair;                 \
        first_type& first_name = this->first;                           \
        second_type& second_name = this->second;                        \
    }

// Demonstration

#include <cstdio>
#include <string>

int main() {
    using status = namedpair(int, code, std::string, message);
    const status result{0, "Success"};
    printf("%s\n", result.message.c_str());
    return result.code;
}
