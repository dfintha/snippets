// static_assert_98.cpp
// Compile-time assertion functions for C++98.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

template <typename BoolToPassVoidToFailT>
struct static_assert_fail {
    static const bool value = BoolToPassVoidToFailT(false);
};

template <bool Condition>
struct static_assert_type {
    typedef bool type;
};

template <>
struct static_assert_type<false> {
    typedef void type;
};

template <bool Condition>
struct static_assert_conditional {
    typedef typename static_assert_type<Condition>::type type;
    static const bool value = static_assert_fail<type>::value;
};

void static_assert_fn(bool) {
}

#define STATIC_ASSERT(condition) \
    static_assert_fn(static_assert_conditional<condition>::value)

// Demonstration

int main() {
    STATIC_ASSERT(true);
    // STATIC_ASSERT(false);
    return 0;
}
