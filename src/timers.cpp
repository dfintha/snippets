// timers.cpp
// Very simple one-shot (timeout) and auto-reload (interval) timer
// implementations utilizing modern C++.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Interface

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <utility>

template <typename Function>
class timeout {
public:
    timeout(std::chrono::seconds delay, Function function) :
        terminated(new std::atomic_bool(false))
    {
        auto worker = std::thread {
            [this, function, delay] () {
                std::this_thread::sleep_for(delay);
                if (!terminated->load())
                    function();
            }
        };
        worker.detach();
    }

    void terminate() {
        terminated->store(true);
    }

private:
    std::shared_ptr<std::atomic_bool> terminated;
};

template <typename Function>
class interval {
public:
    interval(std::chrono::seconds delay, Function function) :
        terminated(new std::atomic_bool(false))
    {
        auto worker = std::thread {
            [this, function, delay] () {
                while (!terminated->load()) {
                    std::this_thread::sleep_for(delay);
                    if (!terminated->load()) {
                        function();
                    } else {
                        break;
                    }
                }
            }
        };
        worker.detach();
    }

    void terminate() {
        terminated->store(true);
    }

private:
    std::shared_ptr<std::atomic_bool> terminated;
};

template <typename Function>
timeout<Function> set_timeout(std::chrono::seconds delay, Function function) {
    return timeout<Function>(delay, function);
}

template <typename Function>
interval<Function> set_interval(std::chrono::seconds delay, Function function) {
    return interval<Function>(delay, function);
}

// Demonstration

#include <cstdio>

int main() {
    using namespace std::chrono_literals;

    auto tu1s = set_timeout(1s, [] () { printf("1-second timeout tick\n"); });
    auto iv5s = set_interval(2s, [] () { printf("2-second interval tick\n"); });
    std::this_thread::sleep_for(5s);
    iv5s.terminate();
    printf("2-second interval terminate\n");

    std::this_thread::sleep_for(2s);
    return 0;
}
