#include "safeshared_rw.h"
#include <iostream>

struct TestObject {
    int value = 0;
    void inc() { ++value; }
};

int main() {
    auto safe = safeshared::SafeSharedRW<TestObject>(new TestObject);

    // THROWING
    safe.write([](TestObject& o) {
        o.inc();
    });

    // NON-THROWING
    if (!safe.try_read([](const TestObject& o) {
        std::cout << o.value << "\n";
    })) {
        std::cout << "object destroyed\n";
    }

    safe.destroy();

    // try_* never throws
    bool ok = safe.try_read([](const TestObject&) {});
    std::cout << "try_read after destroy: " << ok << "\n";

    // throwing version
    try {
        safe.read([](const TestObject&) {});
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << "\n";
    }
}
