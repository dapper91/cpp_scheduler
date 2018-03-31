#pragma once

#include <list>
#include <limits>
#include <functional>

#include <boost/fiber/condition_variable.hpp>
#include <boost/fiber/all.hpp>

#include <engine/base_engine.hpp>


namespace sched {


namespace fibers = boost::fibers;
namespace this_fiber = boost::this_fiber;


class FiberPoolEngine: public BaseEngine {
public:
    typedef fibers::mutex mutex_type;
    typedef fibers::condition_variable condition_variable_type;

    FiberPoolEngine(size_t max_pool_size = std::numeric_limits<size_t>::max()):
        max_pool_size(max_pool_size), stop_flag(false)
    {
    }

    virtual ~FiberPoolEngine()
    {
        shutdown();
        wait();
    }

    void submit(std::function<void()> func) override
    {
        if (stop_flag) {
            throw EngineIsStoped("engine is stoped");
        }
        if (workers.size() >= max_pool_size) {
            throw std::runtime_error("worker pool is full");
        }

        workers.emplace_back([this, func = std::move(func)] {
            func();
            // this_fiber::detach();
            // workers.erase();
        });
    }

    void shutdown() override
    {
        stop_flag = true;
    }

    void wait() override
    {
        for (auto& worker: workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

private:
    bool stop_flag;    
    size_t max_pool_size;
    std::list<fibers::fiber> workers;
};


} // namespace sched