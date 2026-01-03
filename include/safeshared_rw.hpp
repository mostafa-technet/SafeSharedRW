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
    // Constructors
    explicit SafeSharedRW(T* ptr);
    explicit SafeSharedRW(std::shared_ptr<T> ptr);

    // Non-copyable
    SafeSharedRW(const SafeSharedRW&) = delete;
    SafeSharedRW& operator=(const SafeSharedRW&) = delete;

    // Movable
    SafeSharedRW(SafeSharedRW&& other) noexcept;
    SafeSharedRW& operator=(SafeSharedRW&& other) noexcept;

    // Destructor
    ~SafeSharedRW();

    // Read-only access (multiple readers allowed)
    template <typename F>
    void read(F&& fn) const;

    // Mutable access (single writer)
    template <typename F>
    void write(F&& fn);

    // Explicit lifetime check
    bool is_alive() const noexcept;

private:
    void check_alive() const;

    std::shared_ptr<T>      m_ptr;
    std::atomic<bool>       m_alive{false};
    mutable std::shared_mutex m_mutex;
};

// ---------------- Template implementations ----------------

template <typename T>
template <typename F>
void SafeSharedRW<T>::read(F&& fn) const {
    check_alive();
    std::shared_lock lock(m_mutex);
    auto local = m_ptr;           // keep object alive
    fn(*local);
}

template <typename T>
template <typename F>
void SafeSharedRW<T>::write(F&& fn) {
    check_alive();
    std::unique_lock lock(m_mutex);
    auto local = m_ptr;           // keep object alive
    fn(*local);
}

} // namespace safeshared
