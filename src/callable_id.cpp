// callable_id.cpp
// Unique identification of callable entities. Instances of the std::function
// class are identified by the address of their function pointer, if applicable,
// or the underlying type of the callable object, if not. Lambdas are always
// identified by their generated class identifier, and functions are identified
// by their address. As such, a function and a pointer to the same function will
// yield the same identifier. This is intended. Null function pointers, empty
// std::function instances, and the void type have an identifier value of 0.
// Author: Dénes Fintha
// Year: 2019
// -------------------------------------------------------------------------- //

// Interface

#include <cstddef>
#include <functional>
#include <type_traits>
#include <typeinfo>

template <typename Callable>
constexpr size_t generate_callable_id(const Callable& f) noexcept {
    if constexpr (std::is_same<Callable, void>::value) {
        return 0;
    } else if constexpr (std::is_class<Callable>::value) {
        return typeid(Callable).hash_code();
    } else if constexpr (std::is_function<Callable>::value) {
        return generate_callable_id(&f);
    } else {
        if constexpr (std::is_pointer<Callable>::value) {
            if (f == nullptr) return 0;
        }
        return std::hash<Callable>()(f);
    }
}

template <typename Return, typename... Args>
constexpr size_t generate_callable_id(const std::function<Return(Args...)>& f) noexcept {
    using function_ptr_t = Return(*const)(Args...);
    function_ptr_t* target = f.template target<function_ptr_t>();
    if (target != nullptr)
        return generate_callable_id(*target);
    const std::type_info& info = f.target_type();
    if (info == typeid(void))
        return 0;
    return info.hash_code();
}

template <typename Callable>
constexpr bool same_callables(const Callable& lhs, const Callable& rhs) noexcept {
    return generate_callable_id(lhs) == generate_callable_id(rhs);
}

// Demonstration

#include <iostream>

int test_function_1() { return 1; }
int test_function_2() { return 2; }
int test_function_3() { return 3; }

int main() {
    std::function<int()> std_function_lambda = [] () { return 1; };
    std::function<int()> std_function_function_1 = test_function_1;
    std::function<int()> std_function_function_2 = test_function_2;

    // std::function with lambda expression
    std::cout << generate_callable_id(std_function_lambda) << std::endl;

    // functions and function pointers
    std::cout << generate_callable_id(test_function_1) << std::endl;
    std::cout << generate_callable_id(test_function_2) << std::endl;

    // std::function with function pointer
    std::cout << generate_callable_id(std_function_function_1) << std::endl;
    std::cout << generate_callable_id(std_function_function_2) << std::endl;

    // lambda expression
    std::cout << generate_callable_id([] () { return 1; }) << std::endl;

    // function
    std::cout << generate_callable_id(test_function_3) << std::endl;

    // function pointer
    std::cout << generate_callable_id(&test_function_3) << std::endl;

    return 0;
}
