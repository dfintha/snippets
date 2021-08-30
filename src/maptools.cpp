// maptools.cpp
// A few helper functions for std::map-like types.
// Author: Dénes Fintha
// Year: 2020
// -------------------------------------------------------------------------- //

// Interface

template <template <typename, typename> class Map, typename Key, typename Value>
bool contains_key(const Map<Key, Value>& self, const Key& key);

template <template <typename, typename> class Map, typename Key, typename Value, typename Calculation>
const Value& compute_if_absent(Map<Key, Value>& self, const Key& key, Calculation calculation);

// Implementation

template <template <typename, typename> class Map, typename Key, typename Value>
bool contains_key(const Map<Key, Value>& self, const Key& key) {
    const auto end = self.end();
    for (auto seek = self.begin(); seek != end; ++seek) {
        if (seek->first == key)
            return true;
    }
    return false;
}

template <template <typename, typename> class Map, typename Key, typename Value, typename Calculation>
const Value& compute_if_absent(Map<Key, Value>& self, const Key& key, Calculation calculation) {
    if (!contains_key(self, key))
        self[key] = calculation(key);
    return self[key];
}

// Demonstration

#include <cstdlib>
#include <iostream>

int main() {
    std::map<int, long> map;
    const auto calculation = [] (int x) {
        std::cout << "Calculated" << std::endl;
        return long(x);
    };
    compute_if_absent(map, 12, calculation);
    compute_if_absent(map, 12, calculation);
    compute_if_absent(map, 12, calculation);
    return EXIT_SUCCESS;
}
