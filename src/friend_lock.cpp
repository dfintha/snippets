// friend_lock.cpp
// A friend lock can only be instantiated by the type given as its template
// argument. As such, functions taking such arguments can only be called by
// these classes.
// Author: Dénes Fintha
// Year: 2019
// -------------------------------------------------------------------------- //

// Interface

#include <type_traits>

template <typename Friend>
class friend_lock final {
public:
    static_assert(std::is_class_v<Friend>, "friend_lock requires a class type");
    friend Friend;

public:
    friend_lock(const friend_lock&) = delete;
    ~friend_lock() noexcept = default;

private:
    constexpr friend_lock() noexcept = default;
};

// Demonstration

#include <cstdio>

struct subject;
struct subject_friend;

struct subject {
    void foo(friend_lock<subject_friend>) const {
        printf("subject::foo was called\n");
    }
};

struct subject_friend {
    void bar(const subject& target) {
        printf("subject_friend::bar was called\n");
        target.foo(friend_lock<subject_friend>());
    }
};

int main() {
    subject s;
    subject_friend sf;
    sf.bar(s);

    // this should cause a compile-time error
    // s.foo(friend_lock<subject_friend>());
    return 0;
}
