#pragma once

#include <chrono>

#include <base_trigger.hpp>


namespace sched {


template <typename Duration = milliseconds>
class IntervalTrigger: public BaseTrigger {
public:
    IntervalTrigger(Duration interval):
        interval(interval)
    {
    }

    IntervalTrigger(const IntervalTrigger& other):
        interval(other.interval)
    {
    }

    steady_clock::time_point next_fire_time(steady_clock::time_point prev_fire_time = steady_clock::time_point::min()) const override
    {
        if (prev_fire_time == steady_clock::time_point::min()) {
            return steady_clock::now() + interval;
        }
        else {
            return steady_clock::now() + interval - (steady_clock::now() - prev_fire_time);    
        }
        
    }

private:
    Duration interval;
};


} // namespace sched