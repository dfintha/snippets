// higher_order.cpp
// Implementation of some higher order functions in C++98.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

#include <algorithm>

template <typename ContainerT, typename TransformationFn>
ContainerT map(const ContainerT& container, TransformationFn transformation) {
    ContainerT result = container;
    typename ContainerT::iterator it;
    for (it = result.begin(); it != result.end(); ++it) {
        *it = transformation(*it);
    }
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
ValueT foldl(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT accumulator
) {
    return basic_fold(
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
    return basic_fold(
        container.rbegin(),
        container.rend(),
        operation,
        accumulator
    );
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

template <typename ContainerT, typename ValueT, typename BinaryOpFn>
ContainerT scani(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT initial
) {
    return basic_scan(container, operation, initial, true);
}

template <typename ContainerT, typename ValueT, typename BinaryOpFn>
ContainerT scane(
    const ContainerT& container,
    BinaryOpFn operation,
    ValueT initial
) {
    return basic_scan(container, operation, initial, false);
}

// Demonstration

#include <iostream>
#include <vector>

template <typename ValueT>
std::ostream& operator<<(
    std::ostream& stream,
    const std::vector<ValueT>& container
) {
    typename std::vector<ValueT>::const_iterator it = container.begin();
    stream << '[' << *it;
    for (++it; it != container.end(); ++it) {
        stream << ", " << *it;
    }
    stream << ']';
    return stream;
}

struct Mul2Add1 {
    int operator()(int x) {
        return x * 2 + 1;
    }
};

struct IsOdd {
    bool operator()(int x) {
        return x % 2 == 1;
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
    std::cout << "map(v, Mul2Add1()) == " << map(v, Mul2Add1()) << '\n';
    std::cout << "filter(v, IsOdd()) == " << filter(v, IsOdd()) << '\n';

    std::cout << "for_each(v, PrintNumber()) -> ";
    for_each(v, PrintNumber());
    std::cout << '\n';

    std::cout << "foldl(v, LoudAdd(), 0) -> " << foldl(v, LoudAdd(), 0) << '\n';
    std::cout << "foldr(v, LoudAdd(), 0) -> " << foldr(v, LoudAdd(), 0) << '\n';
    std::cout << "scani(v, Add(), 0) == " << scani(v, Add(), 0) << '\n';
    std::cout << "scane(v, Add(), 0) == " << scane(v, Add(), 0) << '\n';

    return 0;
}
