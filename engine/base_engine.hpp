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


class EngineIsFull: public std::runtime_error {
public:
    typedef std::runtime_error super;

    EngineIsFull(const std::string& what_arg):
        super(what_arg)
    {}

    EngineIsFull(const char* what_arg):
        super(what_arg)
    {}
};


class BaseEngine: private boost::noncopyable {
public:    
    typedef std::mutex mutex_type;
    typedef std::condition_variable condition_variable_type;

public:
    void submit(const std::function<void()>& func)
    {
        func();
    }

    void shutdown()
    {        
    }

    void wait()
    {        
    }
};


} // namespace sched