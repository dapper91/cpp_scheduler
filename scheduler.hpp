#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <utility>
#include <algorithm>
#include <atomic>
#include <condition_variable>

#include <boost/core/noncopyable.hpp>
#include <boost/range/adaptor/map.hpp>

#include <job.hpp>
#include <trigger/base_trigger.hpp>
#include <engine/base_engine.hpp>


namespace sched {


using namespace boost;
using namespace std::chrono;


template <typename Engine, typename Duration = milliseconds>
class Scheduler: public boost::noncopyable {
public:    
    Scheduler()
    {
    }

    void start()
    {
        stop_flag = false;
        reload_flag = false;

        build_timeline();

        Duration duration;
        steady_clock::time_point time_point;
        uuids::uuid id;

        while (true) {
            duration = Duration::max();
            time_point = steady_clock::time_point::max();

            if (!timeline.empty()) {
                std::pop_heap(timeline.begin(), timeline.end(), std::greater<std::pair<steady_clock::time_point, uuids::uuid>>());
                std::tie(time_point, id) = timeline.back(); timeline.pop_back();
                duration = duration_cast<Duration>(time_point - steady_clock::now());
            }

            {
                std::unique_lock lock(mx);
                cv.wait_for(lock, duration, [this, time_point] {
                    return (steady_clock::now() >= time_point) || reload_flag || stop_flag;
                });
            }

            if (stop_flag) {
                break;
            }
            else if (reload_flag) {
                build_timeline();
            }
            else {
                const Job& job = job_map.at(id);
                auto next_fire_time = job.trigger().next_fire_time(time_point);

                timeline.push_back({next_fire_time, id});
                std::push_heap(timeline.begin(), timeline.end(), std::greater<std::pair<steady_clock::time_point, uuids::uuid>>());
                
                engine.submit(job.func);
            }
        }       
    }

    void stop()
    {
        stop_flag = true;
        cv.notify_one();
    }

    void shutdown()
    {        
        stop();

        engine.shutdown();
        engine.join();
    }

    void pause()
    {

    }

    void pause(uuids::uuid job_id)
    {

    }

    void resume()
    {

    }

    void resume(uuids::uuid job_id)
    {

    }

    const Job& add_job(Job&& job)
    {
        std::lock_guard lock(mx);

        auto [it, inserted] = job_map.emplace(job.id, std::move(job));
        if (!inserted) {
            throw std::runtime_error("Job already exits");
        }

        reload();
        return (*it).second;
    }

    void remove_job(uuids::uuid job_id)
    {
        std::lock_guard lock(mx);
        
        auto removed = job_map.erase(job_id);
        if (!removed) {
            throw std::runtime_error("Job does not exists");   
        }

        reload();
    }

    const Job& get_job(uuids::uuid job_id)
    {
        return job_map.at(job_id);
    }

    auto get_jobs() -> boost::iterator_range
    {
        return job_map | boost::adaptors::map_values;
    }


private:
    std::vector<std::pair<steady_clock::time_point, uuids::uuid>> timeline;
    std::map<uuids::uuid, Job> job_map;

    Engine engine;

    mutable typename Engine::mutex_type mx;
    typename Engine::condition_variable_type cv;

    std::atomic_bool stop_flag;
    std::atomic_bool reload_flag;

    void build_timeline()
    {
        std::lock_guard lock(mx);

        timeline.clear();

        for (auto& [id, job]: job_map) {
            auto next_fire_time = job.trigger().next_fire_time(steady_clock::time_point::min());
            
            timeline.push_back({next_fire_time, id});
        }

        std::make_heap(timeline.begin(), timeline.end(), std::greater<std::pair<steady_clock::time_point, uuids::uuid>>());
    }

    void reload()
    {
        reload_flag = true;
        cv.notify_one();
    }
};


} // namespace sched