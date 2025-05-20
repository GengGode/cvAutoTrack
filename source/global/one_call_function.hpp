#pragma once
#include <functional>
#include <mutex>

namespace stdex
{
    template <class R, class... Args> struct one_call_function
    {
        std::function<R(Args...)> caller;
        std::mutex mutex;

        void operator=(std::function<R(Args...)> event)
        {
            std::unique_lock lock(mutex);
            this->caller = event;
        }

        R operator()(Args... args)
        {
            std::function<R(Args...)> cache;
            {
                std::unique_lock lock(mutex);
                if (caller == nullptr)
                    return R();
                caller.swap(cache);
            }
            if (cache)
                return cache(std::forward<Args>(args)...);
        }
    };

    template <class R, class... Args> struct one_call_function<R(Args...)> : public one_call_function<R, Args...>
    {
        void operator=(std::function<R(Args...)> event) { one_call_function<R, Args...>::operator=(event); }
    };

    using one_caller = one_call_function<void()>;
} // namespace stdex
