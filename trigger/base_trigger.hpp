#pragma once

#include <chrono>


namespace sched {


using namespace std::chrono;


class BaseTrigger {
public:
    virtual steady_clock::time_point next_fire_time(steady_clock::time_point prev_fire_time) const = 0;
};


} // namespace sched