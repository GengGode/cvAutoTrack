#pragma once
#include <atomic>
#include <mutex>

namespace stdex
{
    template <typename T, bool read_is_lock_free = false> class syncer
    {
        T cache;
        std::mutex cache_mutex;
        std::atomic<bool> no_changed_flag = { true };

    public:
        static_assert(std::is_copy_constructible_v<T>, "T must be copy constructible");
        static_assert(std::is_copy_assignable_v<T>, "T must be copy assignable");

        bool try_sync(T& value)
        {
            if (no_changed_flag.exchange(true))
                return false;
            std::lock_guard lock(cache_mutex);
            value = cache;
            return true;
        }
        T get()
        {
            std::lock_guard lock(cache_mutex);
            return cache;
        }
        void set(const T& value)
        {
            std::lock_guard lock(cache_mutex);
            cache = value;
            no_changed_flag.store(false);
        }
    };
    template <typename T> class syncer<T, true>
    {
        T cache[2] = {};
        std::atomic<bool> readyed_flag = { false };
        std::atomic_flag no_changed_flag = ATOMIC_FLAG_INIT;
        std::mutex setting_mutex;

    public:
        static_assert(std::is_copy_constructible_v<T>, "T must be copy constructible");
        static_assert(std::is_copy_assignable_v<T>, "T must be copy assignable");

        syncer() { no_changed_flag.test_and_set(std::memory_order_release); }

    public:
        bool try_sync(T& value) noexcept
        {
            if (no_changed_flag.test_and_set(std::memory_order_acquire))
                return false;
            auto ready_index = readyed_flag.load(std::memory_order_consume);

            value = cache[ready_index];
            return true;
        }
        T get() noexcept
        {
            auto ready_index = readyed_flag.load(std::memory_order_seq_cst);
            return cache[ready_index];
        }
        void set(const T& value) noexcept
        {
            std::lock_guard lock(setting_mutex);
            const int target_index = 1 - readyed_flag.load(std::memory_order_relaxed);

            cache[target_index] = value;

            readyed_flag.store(target_index, std::memory_order_release);
            no_changed_flag.clear(std::memory_order_release);
        }
    };
} // namespace stdex
