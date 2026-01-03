#include "safeshared_rw.hpp"
#include <iostream>

struct Foo {
    void read() const { std::cout << "read\n"; }
    void write() { std::cout << "write\n"; }
};

int main() {
    safeshared::SafeSharedRW<Foo> safe(new Foo);

    safe.read([](const Foo& f) {
        f.read();
    });

    safe.write([](Foo& f) {
        f.write();
    });
}
