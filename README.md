# C++ scheduler

C++17 scheduler. 

## Dependencies

* C++17
* Boost Range
* Boost Core
* Boost Uuid
* Boost Format
* Boost Fiber (for Fiber Engine only)
* Boost Process (for Process Engine only)



## Usage example

```c++

#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <chrono>
#include <ctime>

#include <scheduler.hpp>
#include <trigger/interval_trigger.hpp>
#include <engine/fiber_engine.hpp>


using namespace std::literals::chrono_literals;


void print(const std::string& message)
{
    std::stringstream message_stream;

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);   

    message_stream << "It's " << std::put_time(std::localtime(&time), "%T") << "\t" << message << std::endl;
    
    std::cout << message_stream.str();
}

int main()
{   
    sched::Scheduler<sched::FiberEngine> scheduler;

    scheduler.add_job({
        std::bind(print, "1 second has been passed"),
        std::make_unique<sched::IntervalTrigger<>>(1s),
        "1 sec interval task"
    });

    scheduler.add_job({
        std::bind(print, "2 seconds have been passed"),
        std::make_unique<sched::IntervalTrigger<>>(2s),
        "2 sec interval task"
    });

    scheduler.add_job({
        std::bind(print, "3 seconds have been passed"),
        std::make_unique<sched::IntervalTrigger<>>(3s),
        "3 sec interval task"
    });
    
    scheduler.start();

    return 0;
}


```


## API

## License

Public Domain License
