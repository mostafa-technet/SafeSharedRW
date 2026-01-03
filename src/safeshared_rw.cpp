#include "safeshared_rw.hpp"

namespace safeshared {

template <typename T>
SafeSharedRW<T>::SafeSharedRW(T* ptr)
    : m_ptr(ptr), m_alive(true) {}

template <typename T>
SafeSharedRW<T>::SafeSharedRW(std::shared_ptr<T> ptr)
    : m_ptr(std::move(ptr)), m_alive(true) {}

template <typename T>
SafeSharedRW<T>::SafeSharedRW(SafeSharedRW&& other) noexcept
    : m_ptr(std::move(other.m_ptr)),
      m_alive(other.m_alive.load(std::memory_order_acquire)) {
    other.m_alive.store(false, std::memory_order_release);
}

template <typename T>
SafeSharedRW<T>& SafeSharedRW<T>::operator=(SafeSharedRW&& other) noexcept {
    if (this != &other) {
        m_ptr = std::move(other.m_ptr);
        m_alive.store(other.m_alive.load(std::memory_order_acquire),
                      std::memory_order_release);
        other.m_alive.store(false, std::memory_order_release);
    }
    return *this;
}

template <typename T>
SafeSharedRW<T>::~SafeSharedRW() {
    m_alive.store(false, std::memory_order_release);
}

template <typename T>
bool SafeSharedRW<T>::is_alive() const noexcept {
    return m_alive.load(std::memory_order_acquire);
}

template <typename T>
void SafeSharedRW<T>::check_alive() const {
    if (!is_alive()) {
        throw std::runtime_error("SafeSharedRW: access after destruction");
    }
}

// Explicit instantiation note:
//
// Because this is a template class, users typically include the
// implementation in the header. If you want to keep a .cpp file,
// you must explicitly instantiate required types here.
//
// Example:
// template class SafeSharedRW<MyType>;

} // namespace safeshared
