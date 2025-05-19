#pragma once
#include <map>
#include <memory>
#include <mutex>

namespace global
{
    namespace detail
    {
        template <typename T> int64_t rc(std::shared_ptr<T> ptr)
        {
            return reinterpret_cast<int64_t>(ptr.get());
        }
    } // namespace detail

    template <typename T> struct shared_pool
    {
        static inline std::map<int64_t, std::shared_ptr<T>> pool = {};
        static inline std::mutex mutex = {};
        static inline std::shared_ptr<T> get(int64_t id)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (auto it = pool.find(id); it != pool.end())
                return it->second;
            return nullptr;
        }
        static inline void remove(int64_t id)
        {
            std::lock_guard<std::mutex> lock(mutex);
            pool.erase(id);
        }
        static inline int64_t set(std::shared_ptr<T> value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto id = detail::rc(value);
            pool[id] = value;
            return id;
        }
    };

    template <typename T> struct shareder
    {
        static inline std::shared_ptr<T> instance = {};
        static inline std::mutex mutex = {};
        template <typename... Args> static inline std::shared_ptr<T> get_or_created_get(Args&&... args)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!instance)
                instance = std::make_shared<T>(std::forward<Args>(args)...);
            return instance;
        }
        static inline std::shared_ptr<T> get()
        {
            std::lock_guard<std::mutex> lock(mutex);
            return instance;
        }
        static inline void set(std::shared_ptr<T> value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            instance = value;
        }
        static inline void reset()
        {
            std::lock_guard<std::mutex> lock(mutex);
            instance.reset();
        }
    };
} // namespace global