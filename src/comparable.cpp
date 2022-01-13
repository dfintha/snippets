// comparable.cpp
// Simple, generic three-way comparison implementation.
// Author: Dénes Fintha
// Year: 2018
// -------------------------------------------------------------------------- //

// Interface

enum comparison_result {
    comparison_lesser = -1,
    comparison_equal = 0,
    comparison_greater = 1
};


template <typename Type>
class comparable {
public:
    virtual comparison_result compare(const comparable& rhs) const = 0;

    bool operator==(const comparable& rhs) const {
        return compare(rhs) == comparison_equal;
    }

    bool operator!=(const comparable& rhs) const {
        return compare(rhs) != comparison_equal;
    }

    bool operator<=(const comparable& rhs) const {
        return compare(rhs) != comparison_greater;
    }

    bool operator>=(const comparable& rhs) const {
        return compare(rhs) != comparison_lesser;
    }

    bool operator<(const comparable& rhs) const {
        return compare(rhs) == comparison_lesser;
    }

    bool operator>(const comparable& rhs) const {
        return compare(rhs) == comparison_greater;
    }

    operator const Type&() const {
        return *dynamic_cast<const Type *>(this);
    }
};

// Demonstration

#include <cstdio>

struct wrapped_int : public comparable<wrapped_int> {
    int x;

    virtual comparison_result compare(const comparable<wrapped_int>& rhs) const {
        const wrapped_int& other = rhs;
        if (x < other.x) return comparison_lesser;
        if (x == other.x) return comparison_equal;
        return comparison_greater;
    }
};

static const char * bool2str(bool x) {
    return x ? "true" : "false";
}

int main() {
    wrapped_int i; i.x = 1;
    wrapped_int j; j.x = 2;

    printf("i = %d\n", i.x);
    printf("j = %d\n", j.x);

    printf("i == j ? %s\n", bool2str(i == j));
    printf("i != j ? %s\n", bool2str(i != j));
    printf("i <= j ? %s\n", bool2str(i <= j));
    printf("i >= j ? %s\n", bool2str(i >= j));
    printf("i < j ? %s\n", bool2str(i < j));
    printf("i > j ? %s\n", bool2str(i > j));

    return 0;
}
