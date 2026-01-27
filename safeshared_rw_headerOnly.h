#pragma once
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <utility>

namespace safeshared {

struct object_destroyed : std::runtime_error {
    object_destroyed() : std::runtime_error("object already destroyed") {}
};

template <typename T>
class SafeSharedRW {
public:
    explicit SafeSharedRW(T* ptr) noexcept
        : ptr_(ptr), alive_(true) {}

    SafeSharedRW(const SafeSharedRW&) = delete;
    SafeSharedRW& operator=(const SafeSharedRW&) = delete;

    ~SafeSharedRW() {
        destroy();
    }

    // -------------------------
    // THROWING API
    // -------------------------

    template <typename F>
    void read(F&& f) const {
        std::shared_lock lock(mutex_);
        if (!alive_.load(std::memory_order_acquire))
            throw object_destroyed{};
        std::forward<F>(f)(*ptr_);
    }

    template <typename F>
    void write(F&& f) {
        std::unique_lock lock(mutex_);
        if (!alive_.load(std::memory_order_acquire))
            throw object_destroyed{};
        std::forward<F>(f)(*ptr_);
    }

    // -------------------------
    // NON-THROWING API
    // -------------------------

    template <typename F>
    bool try_read(F&& f) const noexcept {
        std::shared_lock lock(mutex_, std::try_to_lock);
        if (!lock) return false;
        if (!alive_.load(std::memory_order_acquire))
            return false;
        std::forward<F>(f)(*ptr_);
        return true;
    }

    template <typename F>
    bool try_write(F&& f) noexcept {
        std::unique_lock lock(mutex_, std::try_to_lock);
        if (!lock) return false;
        if (!alive_.load(std::memory_order_acquire))
            return false;
        std::forward<F>(f)(*ptr_);
        return true;
    }

    // -------------------------
    // EXPLICIT DESTROY
    // -------------------------

    void destroy() noexcept {
        std::unique_lock lock(mutex_);
        if (alive_.exchange(false, std::memory_order_acq_rel)) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }

private:
    T* ptr_;
    mutable std::shared_mutex mutex_;
    std::atomic<bool> alive_;
};

} // namespace safeshared
