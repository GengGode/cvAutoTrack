#pragma once
#include <future>
#include <mutex>

namespace stdex
{
    template <typename R> class single_async_executor
    {
        std::future<R> future;
        std::mutex mutex;

    public:
        template <typename Function, typename... Args> void submit_exclusive(Function&& f, Args&&... args)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (future.valid() && future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                return;
            future = std::async(std::launch::async, std::forward<Function>(f), std::forward<Args>(args)...);
        }
        void wait()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (future.valid() /*&& future.wait_for(std::chrono::seconds(0)) != std::future_status::ready*/)
                future.wait();
        }
    };
} // namespace stdex
