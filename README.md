# C++ scheduler

C++17 scheduler. 

## Dependencies

* C++17
* Boost Fiber (for Fiber Engine only)
* Boost Range
* Boost Core
* Boost Uuid
* Boost Format



## Usage example

```c++

#include <iostream>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <csignal>


#include <scheduler.hpp>
#include <trigger/interval_trigger.hpp>
#include <engine/fiber_pool_engine.hpp>

using namespace std::literals::chrono_literals;


void print(const std::string& message)
{
    std::cout << message << std::endl;
}

int main()
{   
    sched::Scheduler<sched::FiberPoolEngine> scheduler;

    scheduler.add_job({
        std::bind(print, "hello"),
        std::make_unique<sched::IntervalTrigger<>>(1s),
        "hello_print"
    });

    scheduler.add_job({
        std::bind(print, "world"),
        std::make_unique<sched::IntervalTrigger<>>(2s),
        "world_print"
    });

    scheduler.add_job({
        std::bind(print, "!!!"),
        std::make_unique<sched::IntervalTrigger<>>(3s),
        "exclaim_print"
    });
    
    scheduler.start();

    return 0;
}



```


## API

## License

Public Domain License
