# SafeSharedRW

SafeSharedRW is a small C++ utility that provides runtime-checked lifetime safety and thread-safe shared access for objects shared across threads.

It is designed to address a common misconception in C++

 `std::shared_ptr` is reference-count thread-safe, but not lifetime-safe at use sites.

This library demonstrates the strongest guarantees that can be achieved at library level (without changing the C++ language).

---

## Motivation

`stdshared_ptr` guarantees
- Atomic reference counting
- Correct destruction when the last owner is released

It does not guarantee
- Safe usage if raw pointers or references escape
- Detection of use-after-free at runtime
- Scoped access discipline
- Clear readwrite concurrency rules

These issues become especially dangerous in multi-threaded code.

`SafeSharedRW` provides
- Scoped access only
- Runtime lifetime checks
- One writer  many readers
- No raw pointer escape

---

## Design Goals

- ✔ Keep the object alive during access
- ✔ Detect access after destruction (runtime)
- ✔ Support multi-threaded readers and a single writer
- ✔ Enforce access through RAII scopes
- ✔ No raw `T` or `T&` exposure

Non-goals
- ❌ Compile-time lifetime enforcement (requires language support)
- ❌ Zero-overhead abstraction
- ❌ Replacing `stdshared_ptr`

---

## Core Idea

Access to the object is only allowed through scoped lambdas

```cpp
safe.read([](const T& obj) {
     safe read-only access
});

safe.write([](T& obj) {
     exclusive mutable access
});
