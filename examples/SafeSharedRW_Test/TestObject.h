#include <iostream>
#include <atomic>

struct TestObject {
    std::atomic<int> value{ 0 };

    TestObject() {
        std::cout << "TestObject constructed\n";
    }

    ~TestObject() {
        std::cout << "TestObject destroyed\n";
    }

    void read() const {
        std::cout << "read value = " << value.load() << "\n";
    }

    void write() {
        ++value;
        std::cout << "write value = " << value.load() << "\n";
    }
};
