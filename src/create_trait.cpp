// create_trait.cpp
// A macro to simplify type trait creation. The developer simply has to specify
// expressions that must be evaluable for the trait to yield a true value. In
// these expressions, the developer can refer to an instance of the subject type
// as 'create_trait_subject_value'.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <type_traits>

template <typename... ExprTypes>
struct evaluable : std::is_void<std::void_t<ExprTypes...>>
{};

#define create_trait_subject_value std::declval<Subject>()

#if defined(create_trait)
#undef create_trait
#endif
#define create_trait(name, ...)                                                \
template <typename Type>                                                       \
class name {                                                                   \
    template <typename Subject>                                                \
    static constexpr auto check(Subject) ->                                    \
        evaluable<decltype(__VA_ARGS__)>;                                      \
    static constexpr auto check(...) ->                                        \
        std::false_type;                                                       \
public:                                                                        \
    static constexpr const bool value =                                        \
        decltype(check(std::declval<Type>()))::value;                          \
}

// Demonstration

#include <iostream>
#include <string>

create_trait(
    is_container,
    create_trait_subject_value.begin(),
    create_trait_subject_value.end()
);

int main() {
    std::cout << is_container<std::string>::value << std::endl;
    std::cout << is_container<std::ostream>::value << std::endl;
    return 0;
}
