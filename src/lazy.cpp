// lazy.cpp
// Lazy evaluation for simple expressions in modern C++.
// Author: Dénes Fintha
// Year: 2024
// -------------------------------------------------------------------------- //

// Interface

#include <type_traits>
#include <utility>

namespace detail {
    template <typename Function>
    struct lazy_impl {
        using return_type = decltype(std::declval<Function>()());

        Function f;

        lazy_impl(Function&& function) : f(std::forward<Function>(function)) {
        }

        operator return_type() {
            return f();
        }
    };

    template <typename Function>
    lazy_impl<Function> make_lazy_impl(Function&& function) {
        return lazy_impl<Function>(std::forward<Function>(function));
    }
}

#define lazy(...) detail::make_lazy_impl([&] { return __VA_ARGS__; })

// Demonstration

#include <cstdio>

int main() {
    puts("Begin");
    auto x = lazy(puts("X"), 2);
    auto y = lazy(puts("Y"), x * 2);
    auto z = lazy(puts("Z"), y * 2);
    puts("End");
    return z;
}