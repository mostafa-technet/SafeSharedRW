#pragma once

#include <memory>
#include <atomic>
#include <shared_mutex>
#include <stdexcept>
#include <utility>

namespace safeshared {

template <typename T>
class SafeSharedRW {
public:
    explicit SafeSharedRW(T* ptr)
        : m_ptr(ptr), m_alive(true) {}

    explicit SafeSharedRW(std::shared_ptr<T> ptr)
        : m_ptr(std::move(ptr)), m_alive(true) {}

    SafeSharedRW(const SafeSharedRW&) = delete;
    SafeSharedRW& operator=(const SafeSharedRW&) = delete;

    SafeSharedRW(SafeSharedRW&& other) noexcept
        : m_ptr(std::move(other.m_ptr)),
          m_alive(other.m_alive.load(std::memory_order_acquire)) {
        other.m_alive.store(false, std::memory_order_release);
    }

    SafeSharedRW& operator=(SafeSharedRW&& other) noexcept {
        if (this != &other) {
            m_ptr = std::move(other.m_ptr);
            m_alive.store(other.m_alive.load(std::memory_order_acquire),
                          std::memory_order_release);
            other.m_alive.store(false, std::memory_order_release);
        }
        return *this;
    }

    ~SafeSharedRW() {
        m_alive.store(false, std::memory_order_release);
    }

    template <typename F>
    void read(F&& fn) const {
        check_alive();
        std::shared_lock lock(m_mutex);
        auto local = m_ptr;
        fn(*local);
    }

    template <typename F>
    void write(F&& fn) {
        check_alive();
        std::unique_lock lock(m_mutex);
        auto local = m_ptr;
        fn(*local);
    }

    bool is_alive() const noexcept {
        return m_alive.load(std::memory_order_acquire);
    }

private:
    void check_alive() const {
        if (!is_alive()) {
            throw std::runtime_error(
                "SafeSharedRW: access after destruction");
        }
    }

    std::shared_ptr<T>        m_ptr;
    std::atomic<bool>        m_alive{false};
    mutable std::shared_mutex m_mutex;
};

} // namespace safeshared
