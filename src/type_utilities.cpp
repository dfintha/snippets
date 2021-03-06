// type_utilities.cpp
// Type traits and related utilities for C++.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <string>
#include <type_traits>
#include <typeinfo>

#if defined(__clang__) || defined(__GNUC__)
#include <cxxabi.h>
#endif

template <typename Type,
          template <typename> typename FirstTrait,
          template <typename> typename... Traits>
struct any_applies :
    public std::conditional<
        FirstTrait<Type>::value,
        std::true_type,
        any_applies<Type, Traits...>
    >::type
{};

template <typename Type,
          template <typename> typename Trait>
struct any_applies<Type, Trait> :
    public Trait<Type>
{};

template <typename Type,
          template <typename> typename... Traits>
constexpr const bool any_applies_v = any_applies<Type, Traits...>::value;

template <typename Type,
          template <typename> typename FirstTrait,
          template <typename> typename... Traits>
struct all_applies :
    public std::conditional<
        !FirstTrait<Type>::value,
        std::false_type,
        all_applies<Type, Traits...>
    >::type
{};

template <typename Type,
          template <typename> typename Trait>
struct all_applies<Type, Trait> :
    public Trait<Type>
{};

template <typename Type,
          template <typename> typename... Traits>
constexpr const bool all_applies_v = all_applies<Type, Traits...>::value;

template <typename Type, typename FirstType, typename... Types>
struct any_of_types :
    public std::conditional<
        std::is_same<Type, FirstType>::value,
        std::true_type,
        any_of_types<Type, Types...>
    >::type
{};

template <typename Type, typename FirstType>
struct any_of_types<Type, FirstType> :
    public std::is_same<Type, FirstType>
{};

template <typename Type, typename... Types>
constexpr const bool any_of_types_v = any_of_types<Type, Types...>::value;

template <typename First, typename Second>
struct decays_to_same :
    public std::is_same<
        typename std::decay<First>::type,
        typename std::decay<Second>::type>
{};

template <typename First, typename Second>
constexpr const bool decays_to_same_v = decays_to_same<First, Second>::value;

template <typename... ExprTypes>
struct evaluable : std::is_void<std::void_t<ExprTypes...>>
{};

template <typename Type>
struct is_container {
private:
    template <typename C>
    static auto check(const C&) ->
        evaluable<
            decltype(std::declval<C>().begin()),
            decltype(std::declval<C>().end())
        >;
    static auto check(...) -> std::false_type;
public:
    static constexpr bool value = decltype(check(std::declval<Type>()))::value;
};

template <typename Type>
constexpr const bool is_container_v = is_container<Type>::value;

template <typename Type>
std::string typename_of() {
    const std::type_info& type = typeid(Type);
    std::string name = type.name();

#if defined(__clang__) || defined(__GNUC__)
    int dummy;
    const char *c_str = name.c_str();
    char *demangled = abi::__cxa_demangle(c_str, nullptr, nullptr, &dummy);
    name = demangled;
    std::free(demangled);
#endif

    while (true) {
        const size_t pos = name.find("> >");
        if (pos == std::string::npos)
            break;
        name.replace(pos, 3, ">>");
    }

    return name;
}

// Demonstration

#include <iostream>
#include <vector>

int main() {
    // these unused volatile values can be checked via the compiler explorer disassembly

    constexpr volatile auto test_any_applies =
        any_applies_v<bool, std::is_floating_point, std::is_integral>;

    constexpr volatile auto test_all_applies =
        all_applies_v<bool, std::is_trivial, std::is_integral>;

    constexpr volatile auto test_any_of_types =
        any_of_types_v<bool, int, float, bool>;

    constexpr volatile auto test_decays_to_same =
        decays_to_same_v< int *, int[12]>;

    constexpr volatile auto test_is_container =
        is_container_v<std::vector<int>>;

    std::string test_typename_of = typename_of<std::vector<std::string>>();
    std::cout << test_typename_of << std::endl;

    return 0;
}
