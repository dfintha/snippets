// delayed_invoke.cpp
// Delayed invoking of callable entities, using std::thread and the std::chrono
// library.
// Author: Dénes Fintha
// Year: 2023
// -------------------------------------------------------------------------- //

// Interface

#include <chrono>
#include <thread>
#include <utility>

template <typename Callable, typename Delay>
class delayed_invoke {
private:
    std::unique_ptr<std::thread> worker;

public:
    template <typename... Arguments>
    delayed_invoke(Callable callee, Delay delay, Arguments&&... arguments) {
        worker = std::unique_ptr<std::thread>(
            new std::thread([&]() {
                std::this_thread::sleep_for(delay);
                callee(std::forward<Arguments&&>(arguments)...);
            }
        ));
    }

    delayed_invoke(const delayed_invoke&) = delete;
    delayed_invoke(delayed_invoke&&) = default;
    delayed_invoke& operator=(const delayed_invoke&) = delete;
    delayed_invoke& operator=(delayed_invoke&&) = default;

    void await() {
        if (worker != nullptr)
            worker->join();
        worker.reset(nullptr);
    }

    ~delayed_invoke() noexcept {
        await();
    }
};

template <typename Callable, typename Delay, typename... Arguments>
delayed_invoke<Callable, Delay>
make_delayed(Callable callee, Delay delay, Arguments&&... arguments) {
    return std::move(
        delayed_invoke<Callable, Delay>(
            callee,
            delay,
            std::forward<Arguments&&>(arguments)...
        )
    );
}

// Demostration

#include <ctime>
#include <iostream>

int main() {
    std::cout << "Start\n";
    const auto before = time(nullptr);
    auto delayed = make_delayed(
        [] (const char *x, time_t t) {
            std::cout << x << ", invoked after ";
            std::cout << (time(nullptr) - t) << "s\n";
        },
        std::chrono::seconds(3),
        "Delayed by 3s",
        before
    );
    std::cout << "Immediate 1\n";
    std::cout << "Immediate 2\n";
    delayed.await();
    std::cout << "Awaited\n";
    return 0;
}
