#pragma once

#include <memory>

#include <boost/core/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

#include <trigger/base_trigger.hpp>


namespace sched {


using boost::uuids::uuid;
namespace uuids = boost::uuids;


class Job: public boost::noncopyable {
public:
    Job(uuid id, std::function<void()>&& func, std::unique_ptr<BaseTrigger>&& trigger_ptr, const std::string& name = "", uint32_t max_instances = 0):
        id(id), 
        func(std::move(func)), 
        trigger_ptr(std::move(trigger_ptr)), 
        name(name.length() == 0 ? (boost::format("job-%1%") % to_string(id)).str() : name), 
        max_instances(max_instances)
    {
    }

    Job(std::function<void()>&& func, std::unique_ptr<BaseTrigger>&& trigger_ptr, const std::string& name = "", uint32_t max_instances = 0):
        Job(uuids::random_generator()(), std::move(func), std::move(trigger_ptr), name, max_instances)        
    {
    }

    Job(Job&& other):
        id(other.id),
        func(std::move(other.func)),
        trigger_ptr(std::move(other.trigger_ptr)), 
        name(other.name),
        max_instances(other.max_instances)
    {
    }

    const BaseTrigger& trigger() const
    {
        return *trigger_ptr;
    }

    const uuid id;
    const std::string name;
    const uint32_t max_instances;
    const std::function<void()> func;

private:
    std::unique_ptr<BaseTrigger> trigger_ptr;    
};


} // namespace sched