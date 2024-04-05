// higher_order.cpp
// Implementation of some higher order functions in C++98 for non-associative
// container types.
// Author: Dénes Fintha
// Year: 2024
// -------------------------------------------------------------------------- //

// Interface

#include <algorithm>
#include <memory>

namespace detail {
    template <typename IteratorT, typename ValueT, typename BinaryOpFn>
    ValueT basic_fold(
        IteratorT where,
        IteratorT end,
        BinaryOpFn operation,
        ValueT accumulator
    ) {
        if (where == end)
            return accumulator;
        const ValueT result = operation(*where++, accumulator);
        return basic_fold(where, end, operation, result);
    }

    template <typename ContainerT, typename ValueT, typename BinaryOpFn>
    ContainerT basic_scan(
        const ContainerT& container,
        BinaryOpFn operation,
        ValueT initial,
        bool inclusive
    ) {
        ContainerT result = container;
        typename ContainerT::iterator current = result.begin();
        typename ContainerT::iterator follow = current;
        if (inclusive) {
            *current++ = operation(*current, initial);
        } else {
            const ValueT temp = *current;
            *current++ = initial;
            initial = temp;
        }
        for (; current != result.end(); ++current, ++follow) {
            if (inclusive) {
                *current = operation(*current, *follow);
            } else {
                const ValueT temp = *current;
                *current = operation(initial, *follow);
                initial = temp;
            }
        }
        return result;
    }

    struct thin_predicate {
        size_t n;
        thin_predicate(size_t factor) : n(factor) {}
        bool operator()(size_t i) {
            return i % n == 0;
        }
    };

    template <template <typename, typename> class ContainerT,
              typename ValueT,
              typename AllocatorT,
              typename TransformationFn>
    struct map_result {
        typedef typename TransformationFn::result_type result_value_t;
        typedef std::allocator<result_value_t> allocator_t;
        typedef ContainerT<result_value_t, allocator_t> type;
    };
}

template <template <typename, typename> class ContainerT,
          typename ValueT,
          typename AllocT,
          typename MapFn>
typename detail::map_result<ContainerT, ValueT, AllocT, MapFn>::type map(
    const ContainerT<ValueT, AllocT>& container,
    MapFn transformation
) {
    typename detail::map_result<ContainerT, ValueT, AllocT, MapFn>::type result;
    std::transform(
        container.begin(),
        container.end(),
        std::back_inserter(result),
        transformation
    );
    return result;
}

template <typename ContainerT, typename UnaryPredicateFn>
ContainerT filter(const ContainerT& container, UnaryPredicateFn predicate) {
    ContainerT result = container;
    result.erase(
        std::remove_if(result.begin(), result.end(), predicate),
        result.end()
    );
    return result;
}

template <typename ContainerT, typename UnaryOpFn>
void for_each(const ContainerT& container, UnaryOpFn operation) {
    typename ContainerT::const_iterator it;
    for (it = container.begin(); it != container.end(); ++it) {
        operation(*it);
    }
}

template <typename ContainerT, typename ValueT, typename BinaryOpFn>
ValueT foldl(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT accumulator
) {
    return detail::basic_fold(
        container.begin(),
        container.end(),
        operation,
        accumulator
    );
}

template <typename ContainerT, typename ValueT, typename BinaryOpFn>
ValueT foldr(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT accumulator
) {
    return detail::basic_fold(
        container.rbegin(),
        container.rend(),
        operation,
        accumulator
    );
}

template <typename ContainerT, typename ValueT, typename BinaryOpFn>
ContainerT scani(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT initial
) {
    return detail::basic_scan(container, operation, initial, true);
}

template <typename ContainerT, typename ValueT, typename BinaryOpFn>
ContainerT scane(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT initial
) {
    return detail::basic_scan(container, operation, initial, false);
}

template <typename ContainerT, typename IndexPredicateFn>
ContainerT ifilter(const ContainerT& container, IndexPredicateFn predicate) {
    ContainerT result;
    typename ContainerT::iterator output = result.end();
    typename ContainerT::const_iterator input = container.begin();
    size_t index = 0;
    while (input != container.end()) {
        typename ContainerT::const_iterator input_end = input;
        std::advance(input_end, 1);
        if (predicate(index)) {
            std::copy(input, input_end, std::inserter(result, output));
            output = result.end();
        }
        std::advance(input, 1);
        ++index;
    }
    return result;
}

template <typename ContainerT>
ContainerT thin(const ContainerT& container, size_t factor) {
    return ifilter<ContainerT>(
        container,
        detail::thin_predicate(factor)
    );
}

// Demonstration

#include <iostream>
#include <vector>

template <template <typename, typename> class ContainerT,
          typename ValueT,
          typename AllocT>
std::ostream& operator<<(
    std::ostream& stream,
    const ContainerT<ValueT, AllocT>& container
) {
    typename ContainerT<ValueT, AllocT>::const_iterator it = container.begin();
    stream << '[' << *it;
    for (++it; it != container.end(); ++it) {
        stream << ", " << *it;
    }
    stream << ']';
    return stream;
}

struct Mul2Add1 {
    typedef int result_type;
    result_type operator()(int x) {
        return x * 2 + 1;
    }
};

struct AsFloat {
    typedef float result_type;
    result_type operator()(int x) {
        return x + 0.1F;
    }
};

struct IsOdd {
    bool operator()(int x) {
        return x % 2 == 1;
    }
};

struct Is2Or5 {
    bool operator()(int x) {
        return x == 2 || x == 5;
    }
};

struct PrintNumber {
    void operator()(int x) {
        std::cout << x << " ";
    }
};

struct LoudAdd {
    int operator()(int x, int y) {
        std::cout << "(" << x << " + " << y << ") ";
        return x + y;
    }
};

struct Add {
    int operator()(int x, int y) {
        return x + y;
    }
};

int main() {
    std::vector<int> v;
    v.push_back(3);
    v.push_back(1);
    v.push_back(4);
    v.push_back(1);
    v.push_back(5);
    v.push_back(9);
    v.push_back(2);
    v.push_back(6);

    std::cout << "v == " << v << '\n';

    // https://en.wikipedia.org/wiki/Map_(higher-order_function)
    std::cout << "map(v, Mul2Add1()) == " << map(v, Mul2Add1()) << '\n';
    std::cout << "map(v, AsFloat()) == " << map(v, AsFloat()) << '\n';

    // https://en.wikipedia.org/wiki/Filter_(higher-order_function)
    std::cout << "filter(v, IsOdd()) == " << filter(v, IsOdd()) << '\n';

    // https://en.wikipedia.org/wiki/Foreach_loop
    std::cout << "for_each(v, PrintNumber()) -> ";
    for_each(v, PrintNumber());
    std::cout << '\n';

    // https://en.wikipedia.org/wiki/Fold_(higher-order_function)
    std::cout << "foldl(v, LoudAdd(), 0) -> " << foldl(v, LoudAdd(), 0) << '\n';
    std::cout << "foldr(v, LoudAdd(), 0) -> " << foldr(v, LoudAdd(), 0) << '\n';

    // https://en.wikipedia.org/wiki/Prefix_sum
    std::cout << "scani(v, Add(), 0) == " << scani(v, Add(), 0) << '\n';
    std::cout << "scane(v, Add(), 0) == " << scane(v, Add(), 0) << '\n';

    // https://en.wikipedia.org/wiki/Downsampling_(signal_processing)
    std::cout << "thin(v, 2) == " << thin(v, 2) << '\n';
    std::cout << "ifilter(v, Is2Or5()) == " << ifilter(v, Is2Or5()) << '\n';

    return 0;
}
