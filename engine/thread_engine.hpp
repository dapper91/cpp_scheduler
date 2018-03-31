#pragma once

#include <limits>
#include <functional>
#include <unordered_map>

#include <mutex>
#include <condition_variable>
#include <atomic>

#include <engine/base_engine.hpp>


namespace sched {


class ThreaeEngine: public BaseEngine {
public:
    typedef std::mutex mutex_type;
    typedef std::condition_variable condition_variable_type;

    ThreaeEngine(size_t max_workers = std::numeric_limits<size_t>::max()):
        workers_cnt(0), max_workers(max_workers), shutdown_flag(false)
    {
    }

    virtual ~ThreaeEngine()
    {
        shutdown();
        wait();
    }

    void submit(const std::function<void()>& func)
    {
        std::unique_lock<mutex_type> lock(mx);

        if (shutdown_flag) {
            throw EngineIsStoped("engine is stoped");
        }
        if (workers_cnt >= max_workers) {
            throw EngineIsFull("worker maximum count has been reached");
        }

        std::thread thread([this, func = std::move(func)] {
            func();
            workers_cnt--;
        });

        workers_cnt++;
        thread.detach();
    }

    void shutdown()
    {
        shutdown_flag = true;
    }

    bool is_shutdown()
    {
        return shutdown_flag;
    }

    void wait()
    {
        std::unique_lock<mutex_type> lock(mx);
        cv.wait(lock, [this] {
            return workers_cnt == 0;
        });
    }

    template <typename Durataion>
    bool wait(Durataion timeout)
    {
        std::unique_lock<mutex_type> lock(mx);
        return cv.wait(lock, timeout, [this] {
            return workers_cnt == 0;
        });
    }

private:
    std::atomic_bool shutdown_flag;
    std::atomic_size_t workers_cnt;
    size_t max_workers;

    mutex_type mx;
    condition_variable_type cv;
};


} // namespace sched