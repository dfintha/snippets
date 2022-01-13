// cpp_utilities.cpp
// Small utility functions that may prove useful in C++.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <algorithm>

template <typename InputIt, typename ForwardIt>
constexpr InputIt find_first_not_of(InputIt first, InputIt last, ForwardIt s_first, ForwardIt s_last) {
    const auto not_in_values = [&s_first, &s_last] (decltype(*first)& check) {
        for (auto it = s_first; it != s_last; ++it) {
            if (check == *it)
                return false;
        }
        return true;
    };

    return std::find_if(first, last, not_in_values);
}

template <typename Type, typename ForwardIt>
Type& trim(Type& original, ForwardIt s_first, ForwardIt s_last) {
    const auto front_cut = find_first_not_of(original.begin(), original.end(), s_first, s_last);
    original.erase(original.begin(), front_cut);
    const auto back_cut = find_first_not_of(original.rbegin(), original.rend(), s_first, s_last);
    original.erase(back_cut.base(), original.end());
    return original;
}

template <typename Callable, typename... Arguments>
inline auto noopt_invoke(const Callable& f, Arguments... args) {
    const volatile auto function = f;
    return function(args...);
}

template <typename Type>
const std::byte * bytes_of(const Type& instance) {
    static_assert(!std::is_pointer<Type>::value);
    const std::byte *bytes = reinterpret_cast<const std::byte *>(&instance);
    return bytes;
}

template <typename Type>
void clear(Type& instance) {
    static constexpr const bool pointer_type = std::is_pointer<Type>::value;
    static constexpr const bool pod_type = std::is_pod<Type>::value;
    static_assert(!pointer_type && pod_type);
    memset(&instance, 0x00, sizeof(Type));
}

// Demonstration

#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

int main() {
    const char whitespaces[] = " \n\t";
    std::string a = "   This string will be trimmed.   ";
    std::cout
        << "'" << a << "' >> "
        << "'" << trim(a, std::begin(whitespaces), std::end(whitespaces)) << "'"
        << std::endl;

    char *string = (char *) malloc(128);
    strcpy(string, "This string will be zeroed out before it's freed.");
    std::cout << string << std::endl;
    noopt_invoke(memset, string, 0x00, 128);
    free(string);

    struct { int x, y, z; } vec3i = { 1, 2, 3 };
    std::cout << '(' << vec3i.x << ", " << vec3i.y << ", " << vec3i.z << ')' << std::endl;
    const auto bytes = bytes_of(vec3i);
    for (size_t i = 0; i < sizeof(vec3i); ++i) {
        std::cout
            << "0x"
            << std::setw(2)
            << std::setfill('0')
            << std::hex
            << int(bytes[i])
            << ' ';
    }
    std::cout << std::endl;
    clear(vec3i);
    std::cout << '(' << vec3i.x << ", " << vec3i.y << ", " << vec3i.z << ')' << std::endl;

    return EXIT_SUCCESS;
}
