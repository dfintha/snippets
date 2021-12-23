// spinlock.cpp
// Spinlock implementation in pure modern C++.
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <atomic>

class spinlock {
public:
    spinlock() = default;
    spinlock(const spinlock&) = delete;
    spinlock& operator=(const spinlock&) = delete;
    ~spinlock() noexcept = default;

    void lock();
    bool try_lock();
    void unlock();

private:
    std::atomic_flag latch = ATOMIC_FLAG_INIT;
};

// Implementation

void spinlock::lock() {
    while (latch.test_and_set(std::memory_order_acquire));
}

bool spinlock::try_lock() {
    return !latch.test_and_set(std::memory_order_acquire);
}

void spinlock::unlock() {
    latch.clear(std::memory_order_release);
}

// Demonstration

#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

std::map<int, double> values;
spinlock values_lock;

void add_value(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::lock_guard<spinlock> guard(values_lock);
    values[x] = 3.14;
}

int main() {
    std::thread t1(add_value, 1);
    std::thread t2(add_value, 2);
    t1.join();
    t2.join();

    for (const auto& pair : values) {
        std::cout << pair.first << " => " << pair.second << '\n';
    }
    return 0;
}
