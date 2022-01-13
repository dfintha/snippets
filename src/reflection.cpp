// reflection.cpp
// Compile-time reflection and conditional call for methods (based on their
// names and argument lists), and members (based on their names).
// Author: Dénes Fintha
// Year: 2018
// -------------------------------------------------------------------------- //

// Interface

#include <type_traits>

template <typename... ExprTypes>
struct evaluable : std::is_void<std::void_t<ExprTypes...>>
{};

#if defined(reflect_method)
#undef reflect_method
#endif
#define reflect_method(name, suffix, ...)                                      \
template <typename T>                                                          \
class has_method_##name##_##suffix {                                           \
    template <typename C>                                                      \
    static constexpr auto check(C) ->                                          \
        evaluable<decltype(std::declval<C>().name(__VA_ARGS__))>;              \
    static constexpr std::false_type check(...);                               \
                                                                               \
public:                                                                        \
    static constexpr bool value =                                              \
        decltype(check(std::declval<T>()))::value;                             \
};                                                                             \
                                                                               \
template <typename T, typename = void>                                         \
struct is_##name##_const_method_##suffix : std::false_type { };                \
                                                                               \
template <typename T>                                                          \
struct is_##name##_const_method_##suffix<T,                                    \
    std::void_t<decltype(std::declval<const T&>().name(__VA_ARGS__))>> :       \
        std::true_type { };                                                    \
                                                                               \
template <typename T, typename I, typename... A>                               \
bool call_##name##_##suffix(T& output, I& instance, A... args) {               \
    if constexpr (!has_method_##name##_##suffix<I>::value) {                   \
        return false;                                                          \
    } else if constexpr (std::is_const_v<I> &&                                 \
                         !is_##name##_const_method_##suffix<I>::value) {       \
        return false;                                                          \
    } else if constexpr (!std::is_void_v<T> &&                                 \
                         !std::is_void_v<                                      \
                            typename std::decay<                               \
                                decltype(instance.name(args...))>::type>) {    \
        output = instance.name(args...);                                       \
        return true;                                                           \
    } else if constexpr (std::is_void_v<                                       \
                            typename std::decay<                               \
                                decltype(instance.name(args...))>::type>) {    \
        instance.name(args...);                                                \
        return true;                                                           \
    }                                                                          \
                                                                               \
    return false;                                                              \
}

#if defined(reflect_member)
#undef reflect_member
#endif
#define reflect_member(name)                                                   \
template <typename T>                                                          \
class has_member_##name {                                                      \
    template <typename C>                                                      \
    static constexpr auto check(C) ->                                          \
            evaluable<decltype(std::declval<C>().name)>;                       \
    static constexpr std::false_type check(...);                               \
                                                                               \
public:                                                                        \
    static constexpr bool value =                                              \
        decltype(check(std::declval<T>()))::value;                             \
};                                                                             \
                                                                               \
template <typename T, typename I>                                              \
bool get_member_##name(T& output, I& instance) {                               \
    if constexpr (!has_member_##name<I>::value) {                              \
        return false;                                                          \
    } else {                                                                   \
        output = instance.name;                                                \
        return true;                                                           \
    }                                                                          \
}                                                                              \
                                                                               \
template <typename T, typename I>                                              \
bool set_member_##name(T& input, I& instance) {                                \
    if constexpr (!has_member_##name<I>::value || std::is_const_v<I>) {        \
        return false;                                                          \
    } else {                                                                   \
        instance.name = input;                                                 \
        return true;                                                           \
    }                                                                          \
}

// Demonstration

#include <iostream>
#include <string>

struct vec3i {
    int x, y, z;
};

reflect_method(begin, noargs);
reflect_member(x);

int main() {
    // these unused volatile values can be checked via the compiler explorer disassembly
    const volatile bool string_has_begin = has_method_begin_noargs<std::string>::value;
    const volatile bool const_string_has_begin = has_method_begin_noargs<const std::string>::value;
    const volatile bool vec3i_has_begin = has_method_begin_noargs<vec3i>::value;
    const volatile bool vec3i_has_member_x = has_member_x<vec3i>::value;
    const volatile bool string_has_member_x = has_member_x<std::string>::value;

    {
        std::string str = "ABC";
        std::string::iterator it;
        if (call_begin_noargs(it, str)) {
            std::cout << "The std::string type does have a begin() function." << std::endl;
            std::cout << "Our string's first letter is '" << *it << "'." << std::endl;
        }
    }

    {
        vec3i vec;
        int it;
        if (!call_begin_noargs(it, vec)) {
            std::cout << "The vec3i type does not have a begin() function." << std::endl;
        }
    }

    {
        vec3i vec = { 1, 2, 3 };
        int x;
        if (get_member_x(x, vec)) {
            std::cout << "The vec3i type does have a member named x." << std::endl;
            std::cout << "Our vec3i's x member is " << x << '.' << std::endl;
        }
    }

    {
        std::string str = "Hello";
        int x;
        if (!get_member_x(x, str)) {
            std::cout << "The std::string type does not have a member named x." << std::endl;
        }
    }

    return 0;
}
