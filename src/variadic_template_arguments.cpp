// variadic_template_arguments.cpp
// Implementation of variadic template arguments in C++98 by using a
// compilation-time list structure.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <iostream>

namespace {
    template <typename Type, Type Value>
    struct helper_integral_constant {
        static const Type value = Value;
    };

    template <typename TypeA, typename TypeB>
    struct helper_is_same : public helper_integral_constant<bool, false> {
    };

    template <typename Type>
    struct helper_is_same<Type, Type> : public helper_integral_constant<bool, true> {
    };

    template <bool Condition, typename IfTrue, typename IfFalse>
    struct helper_conditional {
        typedef IfFalse type;
    };

    template <typename IfTrue, typename IfFalse>
    struct helper_conditional<true, IfTrue, IfFalse> {
        typedef IfTrue type;
    };
}

namespace vta {
    struct end {
        static const size_t length = 0;
        typedef int last;
    };

    template <typename Type, Type Value, typename Rest>
    struct arg {
        static const Type value = Value;
        static const size_t length = Rest::length + 1;
        typedef Rest next;
        typedef arg first;
        typedef typename helper_conditional<helper_is_same<next, end>::value, arg, typename next::last>::type last;
    };

    template <typename List>
    void print() {
        std::cout << List::value << ' ';
        print<typename List::next>();
    }

    template <>
    void print<end>() {
        std::cout << std::endl;
    }
}

// Demonstration

int main() {
    typedef vta::arg<long, 19940720,
                vta::arg<int, 42,
                    vta::arg<char, 'C',
                        vta::end> > > List;

    std::cout << "length = " << List::length << std::endl;
    vta::print<List>();
    return 0;
}
