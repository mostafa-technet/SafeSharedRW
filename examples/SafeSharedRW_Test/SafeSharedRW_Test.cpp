#include "safeshared_rw.hpp"
#include "TestObject.h"
int mainOK() {
    safeshared::SafeSharedRW<TestObject> safe(new TestObject);

    safe.read([](const TestObject& o) {
        o.read();
        });

    safe.write([](TestObject& o) {
        o.write();
        });
}

    int mainMisuse() {
        safeshared::SafeSharedRW<TestObject>* p;

        try {
            {
                safeshared::SafeSharedRW<TestObject> tmp(new TestObject);
                p = &tmp;
            } // tmp destroyed here

            p->read([](const TestObject&) {});
        }
        catch (const std::exception& e) {
            std::cout << "Caught: " << e.what() << "\n";
        }
    }

    int main()
    {
        mainMisuse();
    }
