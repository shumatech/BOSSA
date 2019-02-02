#include <chrono>
#include <thread>

// Replacement for sleep
void inline
usleep(int length) {
    std::this_thread::sleep_for(std::chrono::milliseconds(length));
}
