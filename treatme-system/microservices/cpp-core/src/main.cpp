
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "[*] C++ Engine is running and waiting for RabbitMQ messages..." << std::endl;
    // حلقة لانهائية لمحاكاة عمل الـ Consumer المستمر
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
