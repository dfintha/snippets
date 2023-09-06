// maputils.cpp
// A few helper functions for associative container types (std::map and
// std::unordered_map), inspired by the Python and Java languages. Compatible
// with C++98.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

// Returns true if the given key is contained in the container.
// Based on Java's 'java.util.Map.containsKey' function.
template <typename MapT, typename KeyT>
bool contains_key(MapT& container, const KeyT& key) {
    return container.find(key) != container.end();
}

// Returns true if the given value is contained in the container under any key.
// Based on Java's 'java.util.Map.containsValue' function.
template <typename MapT, typename ValueT>
bool contains_value(MapT& container, const ValueT& value) {
    typename MapT::const_iterator it;
    for (it = container.begin(); it != container.end(); ++it) {
        if (it->second == value)
            return true;
    }
    return false;
}

// Updates items in the container with elements from another one.
// Based on Python's 'dict.update' function.
template <typename MapT>
MapT& update(MapT& lhs, const MapT& rhs) {
    typename MapT::const_iterator it;
    for (it = rhs.begin(); it != rhs.end(); ++it) {
        lhs[it->first] = it->second;
    }
    return lhs;
}

// Creates a new container with the given keys, all set to the same value.
// Based on Python's 'dict.fromkeys' function.
template <typename MapT, typename KeyContainerT, typename ValueT>
MapT from_keys(const KeyContainerT& keys, const ValueT& value) {
    MapT result;
    typename KeyContainerT::const_iterator it;
    for (it = keys.begin(); it != keys.end(); ++it) {
        result[*it] = value;
    }
    return result;
}

// Computes and adds a value to the container, if it is not already present.
// Based on Java's 'java.util.Map.computeIfAbsent' method.
template <typename MapT, typename KeyT, typename CalculateFn>
typename MapT::const_iterator compute_if_absent(
    MapT& container,
    const KeyT& key,
    CalculateFn calculate_fn
) {
    if (!contains_key(container, key))
        container[key] = calculate_fn(key);
    return container.find(key);
}

// Merges two container, adding the contained values to the result. If the
// same key is present in both of them, merge_fn is called to choose from them.
// The merge_fn function takes the key, and both values as its argument and
// shall return the chosen value.
template <typename MapT, typename MergeFn>
MapT merge(const MapT& lhs, const MapT& rhs, MergeFn merge_fn) {
    MapT result;
    typename MapT::const_iterator it;

    for (it = lhs.begin(); it != lhs.end(); ++it) {
        if (contains_key(rhs, it->first)) {
            result[it->first] = merge_fn(
                it->first,
                it->second,
                rhs.at(it->first)
            );
        } else {
            result[it->first] = it->second;
        }
    }

    for (it = rhs.begin(); it != rhs.end(); ++it) {
        if (!contains_key(lhs, it->first))
            result[it->first] = it->second;
    }

    return result;
}

// Demonstration

#include <iostream>
#include <map>
#include <vector>

struct ComputeAlphabetFn {
    typedef int value_type;
    char operator()(int x) {
        std::cout << "A calculation was performed.\n";
        return char(x + 'A' - 1);
    }
};

struct MergeGreaterFn {
    char operator()(int, char a, char b) {
        return a > b ? a : b;
    }
};

template <typename MapT>
static void print_map(const MapT& container) {
    typename MapT::const_iterator it = container.begin();
    std::cout << "{ " << it->first << ": " << it->second;
    for (++it; it != container.end(); ++it) {
        std::cout << ", " << it->first << ": " << it->second;
    }
    std::cout << " }\n";
}

int main() {
    std::map<int, char> subject;
    for (int i = 1; i < 12; ++i) {
        subject[i] = char('A' + i - 1);
    }
    std::cout << "Initially, the map contains the data below.\n";
    print_map(subject);
    std::cout << '\n';

    for (int i = 10; i < 15; ++i) {
        const int key = i;
        const char *yn = contains_key(subject, key) ? "Yes\n" : "No\n";
        std::cout << "Is the key " << key << " present in it? " << yn;
    }
    std::cout << '\n';

    for (int i = 10; i < 15; ++i) {
        const char value = i + 'A';
        const char *yn = contains_value(subject, value) ? "Yes\n" : "No\n";
        std::cout << "Is the value " << value << " present in it? " << yn;
    }
    std::cout << '\n';

    std::cout << "The 'compute_if_absent' function will be called on this\n";
    std::cout << "Map three times, with the key 12 every time.\n";
    std::cout << "Each performed calculation emits an output.\n";
    compute_if_absent(subject, 12, ComputeAlphabetFn());
    compute_if_absent(subject, 12, ComputeAlphabetFn());
    compute_if_absent(subject, 12, ComputeAlphabetFn());
    std::cout << "Now, the content of the map is the data below.\n";
    print_map(subject);
    std::cout << '\n';

    std::vector<int> keys;
    keys.push_back(13);
    keys.push_back(14);
    keys.push_back(15);
    std::map<int, char> addition = from_keys< std::map<int, char> >(keys, 'X');
    std::cout << "A new map was created using from_keys, with keys being\n";
    std::cout << "[13, 14, 15], and the value being 'X'.\n";
    print_map(addition);
    std::cout << '\n';

    std::cout << "Now the above new values are added to the map using the\n";
    std::cout << "update function.\n";
    update(subject, addition);
    print_map(subject);
    std::cout << '\n';

    std::cout << "Another new map is created, with the keys [14, 15, 16,\n";
    std::cout << "17], and values ['W', 'Z', 'W', 'Z'], and then merged to\n";
    std::cout << "our subject with the merge function. The choice between the\n";
    std::cout << "two present values will always be the greater one.\n";
    addition.clear();
    addition[14] = 'W';
    addition[15] = 'Z';
    addition[16] = 'W';
    addition[17] = 'Z';
    std::map<int, char> result = merge(subject, addition, MergeGreaterFn());
    print_map(result);
    std::cout << '\n';

    return 0;
}
