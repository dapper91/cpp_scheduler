#pragma once

#include <thread>
#include <functional>

#include <boost/core/noncopyable.hpp>


namespace sched {


class EngineIsStoped: public std::runtime_error {
public:
    typedef std::runtime_error super;

    EngineIsStoped(const std::string& what_arg):
        super(what_arg)
    {}

    EngineIsStoped(const char* what_arg):
        super(what_arg)
    {}
};


class BaseEngine: private boost::noncopyable {
public:    
    typedef std::mutex mutex_type;
    typedef std::condition_variable condition_variable_type;

    virtual void submit(std::function<void()> func) = 0;
    virtual void shutdown() = 0;
    virtual void wait() = 0;
};


class BlockingEngine: public BaseEngine {
public:
    void submit(std::function<void()> func) override
    {
        func();
    }

    void shutdown() override
    {        
    }

    void wait() override
    {        
    }
};


} // namespace sched