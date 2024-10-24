// type_utilities.cpp
// Type traits and related utilities for C++ (C++17 and above).
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

namespace detail {
    template <typename... NewArgs,
              template <typename...> typename Class,
              typename... OldArgs>
    auto replace_template_arguments(const Class<OldArgs...>&) -> Class<NewArgs...>;

    template <template <typename...> typename NewClass,
              template <typename...> typename OldClass,
              typename... Args>
    auto replace_template(const OldClass<Args...>&) -> NewClass<Args...>;
}

template <typename Class, typename... NewArgs>
struct replace_template_arguments {
    using type = decltype(
        detail::replace_template_arguments<NewArgs...>(std::declval<Class>())
    );
};

template <typename Class, typename... NewArgs>
using replace_template_arguments_t =
    typename replace_template_arguments<Class, NewArgs...>::type;

template <template <typename...> typename NewClass, typename OldClass>
struct replace_template {
    using type = decltype(
        detail::replace_template<NewClass>(std::declval<OldClass>())
    );
};

template <template <typename...> typename NewClass, typename OldClass>
using replace_template_t =
    typename replace_template<NewClass, OldClass>::type;

template <template <typename...> typename OldClass,
          template <typename...> typename NewClass,
          typename... Args>
struct replace_template {
    using type = NewClass<Args...>;
};

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

namespace detail {
    template <typename Type>
    struct cv_prefix_of {
        static constexpr const char *value = "";
    };

    template <typename Type>
    struct cv_prefix_of<const Type> {
        static constexpr const char *value = "const ";
    };

    template <typename Type>
    struct cv_prefix_of<volatile Type> {
        static constexpr const char *value = "volatile ";
    };

    template <typename Type>
    struct cv_prefix_of<const volatile Type> {
        static constexpr const char *value = "const volatile ";
    };

    template <typename Type>
    constexpr const char *cv_prefix_of_v = cv_prefix_of<Type>::value;

    template <typename Type>
    struct ref_suffix_of {
        static constexpr const char *value = "";
    };

    template <typename Type>
    struct ref_suffix_of<Type&> {
        static constexpr const char *value = "&";
    };

    template <typename Type>
    struct ref_suffix_of<Type&&> {
        static constexpr const char *value = "&&";
    };

    template <typename Type>
    constexpr const char *ref_suffix_of_v = ref_suffix_of<Type>::value;
}

template <typename Type>
std::string typename_of() {
    static const std::string mangled = typeid(Type).name();
    static bool initialized = false;
    static std::string demangled;

    if (initialized)
     return demangled;

#if defined(__GLIBCXX__)
    int status;
    char *buffer = abi::__cxa_demangle(
        mangled.c_str(),
        nullptr,
        nullptr,
        &status
    );
    if (status == 0) {
        demangled = buffer;
        free(buffer);
    } else {
        demangled = mangled;
    }
#else
    demangled = mangled;
#endif

    while (true) {
        const size_t pos = demangled.find("> >");
        if (pos == std::string::npos)
            break;
        demangled.replace(pos, 3, ">>");
    }

    const char *prefix = detail::cv_prefix_of_v<std::remove_reference_t<Type>>;
    const char *suffix = detail::ref_suffix_of_v<Type>;
    demangled = prefix + demangled + suffix;

    initialized = true;
    return demangled;
}

// Demonstration

#include <cstdio>
#include <vector>

#define TEST_BOOL(...)                                                  \
    do {                                                                \
        constexpr bool conditional = __VA_ARGS__;                       \
        printf(#__VA_ARGS__ ": %s\n", conditional ? "true" : "false");  \
    } while (false)

#define TEST_STRING(...)                                \
    do {                                                \
        const auto result = __VA_ARGS__;                \
        printf(#__VA_ARGS__ ": %s\n", result.c_str());  \
    } while (false)

int main() {
    TEST_BOOL(any_applies_v<bool, std::is_floating_point, std::is_integral>);
    TEST_BOOL(any_applies_v<bool, std::is_floating_point, std::is_function>);
    fputc('\n', stdout);

    TEST_BOOL(all_applies_v<bool, std::is_trivial, std::is_integral>);
    TEST_BOOL(all_applies_v<bool, std::is_function, std::is_integral>);
    fputc('\n', stdout);

    TEST_BOOL(any_of_types_v<bool, int, float, bool>);
    TEST_BOOL(any_of_types_v<bool, int, float, double>);
    fputc('\n', stdout);

    TEST_BOOL(decays_to_same_v<int *, int[12]>);
    TEST_BOOL(decays_to_same_v<int *, double[12]>);
    fputc('\n', stdout);

    TEST_BOOL(is_container_v<std::vector<int>>);
    TEST_BOOL(is_container_v<int>);
    fputc('\n', stdout);

    std::vector<int> int_vector;
    replace_template_arguments_t<decltype(int_vector), double> double_vector;
    TEST_STRING(typename_of<decltype(int_vector)>());
    TEST_STRING(typename_of<decltype(double_vector)>());

    const auto& int_vector_cref = int_vector;
    const volatile auto& int_vector_cvref = int_vector;
    TEST_STRING(typename_of<decltype(int_vector_cref)>());
    TEST_STRING(typename_of<decltype(int_vector_cvref)>());
    TEST_STRING(typename_of<volatile int&&>());

    return 0;
}
