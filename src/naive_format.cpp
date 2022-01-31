// naive_format.cpp
// Simple, replacement-based, in-place formatting function, which takes its
// placeholder sequence as a function argument.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <cstring>
#include <string>

std::string to_string(char x);

std::string to_string(const char *s);

std::string to_string(const std::string& s);

std::string&& format(std::string&& str, const char *);

template <typename First, typename... Rest>
std::string&& format(std::string&& str, const char *seq, const First& first, const Rest&... rest) {
    using std::to_string;
    using ::to_string;
    const size_t where = str.find(seq);
    if (where == std::string::npos)
        return std::move(str);
    str.replace(where, strlen(seq), to_string(first));
    return std::move(format(std::move(str), seq, rest...));
}

// Implementation

std::string to_string(char x) {
    const char result[] = { x, '\0'};
    return result;
}

std::string to_string(const char *s) {
    return s;
}

std::string to_string(const std::string& s) {
    return s;
}

std::string&& format(std::string&& str, const char *) {
    return std::move(str);
}

// Demonstration

#include <iostream>

int main() {
    using namespace std::literals;
    std::cout << format("Hello, {}! {}"s, "{}", "world", 42) << std::endl;
    return EXIT_SUCCESS;
}
