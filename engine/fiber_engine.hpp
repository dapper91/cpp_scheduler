#pragma once

#include <limits>
#include <functional>
#include <unordered_map>
#include <atomic>

#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/all.hpp>

#include <engine/base_engine.hpp>


namespace sched {


namespace fibers = boost::fibers;
namespace this_fiber = boost::this_fiber;


class FiberEngine: public BaseEngine {
public:
    typedef fibers::mutex mutex_type;
    typedef fibers::condition_variable condition_variable_type;

    FiberEngine(size_t max_workers = std::numeric_limits<size_t>::max()):
        workers_cnt(0), max_workers(max_workers), shutdown_flag(false)
    {
    }

    virtual ~FiberEngine()
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

        fibers::fiber fiber([this, func = std::move(func)] {
            func();
            workers_cnt--;
        });

        workers_cnt++;
        fiber.detach();
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