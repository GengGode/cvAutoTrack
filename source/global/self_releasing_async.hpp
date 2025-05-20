#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stop_token>
#include <string>
#include <thread>
#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
    #undef NOMINMAX
    #undef WIN32_LEAN_AND_MEAN
#endif
namespace stdex
{
    namespace detail
    {
        struct async_task
        {
            std::string name;
            std::thread::id id;
            std::shared_future<void> future;
        };
        class async_pool
        {
            std::map<std::thread::id, async_task> tasks;
            std::shared_mutex tasks_mutex;

        private:
            std::jthread runner = std::jthread([this](std::stop_token st) { this->run(st); });
            std::stop_source runner_stop_source = runner.get_stop_source();
            std::condition_variable runner_cv;
            std::mutex runner_mutex;

        private:
            void run(std::stop_token st)
            {
                while (!st.stop_requested())
                {
                    {
                        std::unique_lock lock(tasks_mutex);

                        std::vector<std::thread::id> ids;
                        for (auto& [id, task] : tasks)
                            if (task.future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
                                ids.push_back(id);

                        for (auto& id : ids)
                        {
                            tasks.erase(id);
                            forever_stop_sources.erase(id);
                        }
                    }
                    std::unique_lock run_lock(runner_mutex);
                    runner_cv.wait_for(run_lock, std::chrono::milliseconds(100), [&st] { return st.stop_requested(); });
                }
            }

            void destroy()
            {
                {
                    std::shared_lock lock(tasks_mutex);
                    for (auto& [id, stop_source] : forever_stop_sources)
                        stop_source.request_stop();
                }
                runner_cv.notify_all();
                std::unique_lock run_lock(runner_mutex);
            }

            std::thread::id register_task(std::string name, std::thread::id id, std::shared_future<void> future)
            {
                {
                    std::unique_lock lock(tasks_mutex);
                    tasks[id] = { name, id, future };
                }
                runner_cv.notify_all();
                return id;
            }

        public:
            async_pool() = default;
            ~async_pool() { destroy(); }

        public:
            template <typename Fn, typename... Args> std::thread::id start(std::string name, Fn&& fn, Args&&... args)
            {
                std::promise<std::thread::id> id_promise;
                auto task = std::async(std::launch::async, [this, &id_promise, fn = std::forward<Fn>(fn), ... args = std::forward<Args>(args)]() mutable {
                    id_promise.set_value(std::this_thread::get_id());
                    fn(std::forward<Args>(args)...);
                });

                return register_task(name, id_promise.get_future().get(), task.share());
            }
            template <typename Fn, typename... Args> std::thread::id start_wait(std::string name, std::chrono::milliseconds wait_time, Fn&& fn, Args&&... args)
            {
                std::promise<std::thread::id> id_promise;

                auto start_time = std::chrono::high_resolution_clock::now() + wait_time;
                auto task = std::async(std::launch::async, [this, &id_promise, start_time, fn = std::forward<Fn>(fn), ... args = std::forward<Args>(args)]() mutable {
                    id_promise.set_value(std::this_thread::get_id());
                    auto stop_token = runner_stop_source.get_token();
                    while (stop_token.stop_requested() == false)
                    {
                        if (std::chrono::high_resolution_clock::now() >= start_time)
                        {
                            fn(std::forward<Args>(args)...);
                            break;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                });

                return register_task(name, id_promise.get_future().get(), task.share());
            }

        private:
            std::map<std::thread::id, std::stop_source> forever_stop_sources;

            std::thread::id register_task(std::string name, std::thread::id id, std::shared_future<void> future, std::stop_source forever_stop_source)
            {
                std::unique_lock lock(tasks_mutex);
                tasks[id] = { name, id, future };
                forever_stop_sources[id] = forever_stop_source;
                runner_cv.notify_all();
                return id;
            }

        public:
            template <typename Fn, typename... Args> std::thread::id start_forever(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
            {
                std::promise<std::thread::id> id_promise;
                std::stop_source forever_stop_source;
                auto forever_stop_token = forever_stop_source.get_token();

                auto task = std::async(std::launch::async,
                                       [this, &id_promise, forever_stop_token = std::move(forever_stop_token), interval, fn = std::forward<Fn>(fn), ... args = std::forward<Args>(args)]() mutable {
                                           id_promise.set_value(std::this_thread::get_id());

                                           std::condition_variable cv;
                                           std::mutex m;
                                           auto next_until = std::chrono::high_resolution_clock::now() + interval;

                                           auto stop_token = runner_stop_source.get_token();
                                           while (!forever_stop_token.stop_requested() && !stop_token.stop_requested())
                                           {
                                               fn(std::forward<Args>(args)...);
                                               std::unique_lock lock(m);
                                               cv.wait_until(lock, next_until, [&stop_token, &forever_stop_token] { return stop_token.stop_requested() || forever_stop_token.stop_requested(); });
                                               next_until += interval;
                                           }
                                       });

                return register_task(name, id_promise.get_future().get(), task.share(), forever_stop_source);
            }
            template <typename Fn, typename... Args> std::thread::id start_forever_high_resolution(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
            {
                std::promise<std::thread::id> id_promise;
                std::stop_source forever_stop_source;
                auto forever_stop_token = forever_stop_source.get_token();

                auto task = std::async(std::launch::async,
                                       [this, &id_promise, forever_stop_token = std::move(forever_stop_token), interval, fn = std::forward<Fn>(fn), ... args = std::forward<Args>(args)]() mutable {
                                           id_promise.set_value(std::this_thread::get_id());
#if defined(_WIN32) || defined(_WIN64)
                                           timeBeginPeriod(1);
#endif
                                           auto next_until = std::chrono::high_resolution_clock::now() + interval;

                                           auto stop_token = runner_stop_source.get_token();
                                           while (!forever_stop_token.stop_requested() && !stop_token.stop_requested())
                                           {
                                               fn(std::forward<Args>(args)...);

                                               while (!forever_stop_token.stop_requested() && !stop_token.stop_requested())
                                               {
                                                   auto now = std::chrono::high_resolution_clock::now();
                                                   if (now >= next_until)
                                                       break;
                                                   auto remaining = next_until - now;
                                                   if (remaining > std::chrono::milliseconds(2))
                                                       std::this_thread::sleep_for(std::chrono::milliseconds(1));
                                                   else
                                                       std::this_thread::yield();
                                               }

                                               next_until += interval;
                                           }
#if defined(_WIN32) || defined(_WIN64)
                                           timeEndPeriod(1);
#endif
                                       });

                return register_task(name, id_promise.get_future().get(), task.share(), forever_stop_source);
            }

            template <typename Fn, typename... Args> std::thread::id start_forever_system_perf(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
            {
                std::promise<std::thread::id> id_promise;
                std::stop_source forever_stop_source;
                auto forever_stop_token = forever_stop_source.get_token();
                auto task = std::async(std::launch::async,
                                       [this, &id_promise, forever_stop_token = std::move(forever_stop_token), interval, fn = std::forward<Fn>(fn), ... args = std::forward<Args>(args)]() mutable {
                                           id_promise.set_value(std::this_thread::get_id());
#if defined(_WIN32) || defined(_WIN64)
                                           timeBeginPeriod(1);
                                           LARGE_INTEGER freq;
                                           ::QueryPerformanceFrequency(&freq);
                                           const LONGLONG interval_counts = (interval.count() * freq.QuadPart) / 1000;

                                           LARGE_INTEGER next_time;
                                           ::QueryPerformanceCounter(&next_time);
                                           next_time.QuadPart += interval_counts;
                                           auto stop_token = runner_stop_source.get_token();
                                           while (!forever_stop_token.stop_requested() && !stop_token.stop_requested())
                                           {
                                               fn(std::forward<Args>(args)...);
                                               while (!forever_stop_token.stop_requested() && !stop_token.stop_requested())
                                               {
                                                   LARGE_INTEGER current_time;
                                                   ::QueryPerformanceCounter(&current_time);
                                                   if (current_time.QuadPart >= next_time.QuadPart)
                                                       break;
                                                   const LONGLONG remaining = next_time.QuadPart - current_time.QuadPart;
                                                   const double remaining_ms = (remaining * 1000.0) / freq.QuadPart;
                                                   if (remaining_ms > 2.0)
                                                       std::this_thread::sleep_for(std::chrono::milliseconds(1));
                                                   else
                                                       ::Sleep(0); // more high resolution wait, high cpu used.
                                               }
                                               next_time.QuadPart += interval_counts;
                                           }
                                           timeEndPeriod(1);
#endif
                                       });
                return register_task(name, id_promise.get_future().get(), task.share(), forever_stop_source);
            }

        public:
            std::vector<async_task> names(std::string name)
            {
                std::vector<async_task> result;
                std::shared_lock lock(tasks_mutex);
                for (auto& [_, task] : tasks)
                    if (task.name == name)
                        result.push_back(task);
                return result;
            }
            std::shared_ptr<async_task> id(std::thread::id id)
            {
                std::shared_lock lock(tasks_mutex);
                auto task = tasks.find(id);
                if (task == tasks.end())
                    return nullptr;
                return std::make_shared<async_task>(task->second);
            }

        public:
            void wait(std::thread::id id)
            {
                auto task = this->id(id);
                if (task == nullptr)
                    return;
                task->future.wait();
            }
            void stop_forever(std::thread::id id)
            {
                auto forever_stop_source = forever_stop_sources.find(id);
                if (forever_stop_source == forever_stop_sources.end())
                    return;
                forever_stop_source->second.request_stop();
            }
        };

        static inline async_pool default_pool;
    } // namespace detail

    struct self_releasing_async
    {
        stdex::detail::async_pool& pool;

        self_releasing_async() : pool(stdex::detail::default_pool) {}
        self_releasing_async(stdex::detail::async_pool& pool) : pool(pool) {}
        ~self_releasing_async() = default;

        bool has(std::string name) { return !pool.names(name).empty(); }
        bool has(std::thread::id id) { return pool.id(id) != nullptr; }
        void wait(std::thread::id id) { pool.wait(id); }
        void stop_forever(std::thread::id id) { pool.stop_forever(id); }

        template <typename Fn, typename... Args> std::thread::id start(std::string name, Fn&& fn, Args&&... args) { return pool.start(name, std::forward<Fn>(fn), std::forward<Args>(args)...); }
        template <typename Fn, typename... Args> std::thread::id start_wait(std::string name, std::chrono::milliseconds wait_time, Fn&& fn, Args&&... args)
        {
            return pool.start_wait(name, wait_time, std::forward<Fn>(fn), std::forward<Args>(args)...);
        }
        template <typename Fn, typename... Args> std::thread::id start_forever(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
        {
            return pool.start_forever(name, interval, std::forward<Fn>(fn), std::forward<Args>(args)...);
        }
        template <typename Fn, typename... Args> std::thread::id start_forever_high_resolution(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
        {
            return pool.start_forever_high_resolution(name, interval, std::forward<Fn>(fn), std::forward<Args>(args)...);
        }
        template <typename Fn, typename... Args> std::thread::id start_forever_system_perf(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
        {
            return pool.start_forever_system_perf(name, interval, std::forward<Fn>(fn), std::forward<Args>(args)...);
        }
    };

    bool has(std::string name)
    {
        return !stdex::detail::default_pool.names(name).empty();
    }
    bool has(std::thread::id id)
    {
        return stdex::detail::default_pool.id(id) != nullptr;
    }
    std::shared_ptr<stdex::detail::async_task> id(std::thread::id id)
    {
        return stdex::detail::default_pool.id(id);
    }
    void stop_forever(std::thread::id id)
    {
        stdex::detail::default_pool.stop_forever(id);
    }
    template <typename Fn, typename... Args> std::thread::id start(std::string name, Fn&& fn, Args&&... args)
    {
        return stdex::detail::default_pool.start(name, std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
    template <typename Fn, typename... Args> std::thread::id start_wait(std::string name, std::chrono::milliseconds wait_time, Fn&& fn, Args&&... args)
    {
        return stdex::detail::default_pool.start_wait(name, wait_time, std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
    template <typename Fn, typename... Args> std::thread::id start_forever(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
    {
        return stdex::detail::default_pool.start_forever(name, interval, std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
    template <typename Fn, typename... Args> std::thread::id start_forever_high_resolution(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
    {
        return stdex::detail::default_pool.start_forever_high_resolution(name, interval, std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
    template <typename Fn, typename... Args> std::thread::id start_forever_system_perf(std::string name, std::chrono::milliseconds interval, Fn&& fn, Args&&... args)
    {
        return stdex::detail::default_pool.start_forever_system_perf(name, interval, std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
} // namespace stdex
