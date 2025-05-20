//
// Created by Mirco Theile on 17/5/25.
//

#ifndef THREADPOOLSCHEDULER_H
#define THREADPOOLSCHEDULER_H

#include <unordered_set>
#include <thread>

#include "cpsCore/Utilities/Scheduler/IScheduler.h"
#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Configuration/ConfigurableObject.hpp"
#include "cpsCore/Synchronization/IRunnableObject.h"

struct Task;

struct ThreadPoolSchedulerParams
{
    Parameter<int> numThreads = {4, "num_threads", true};

    template <class Configurator>
    void
    configure(Configurator& c)
    {
        c & numThreads;
    }
};

class ThreadPoolScheduler : public IScheduler,
                            public AggregatableObject<ITimeProvider>,
                            public ConfigurableObject<ThreadPoolSchedulerParams>,
                            public IRunnableObject
{
public:
    static constexpr auto typeId = "pool";

    Event
    schedule(const TaskHandle& task, Duration initialFromNow, const std::string& eventName) override;
    Event
    schedule(const TaskHandle& task, Duration initialFromNow, Duration period, const std::string& eventName) override;
    void
    stop() override;
    void
    setMainThread() override;
    void
    startSchedule() override;
    bool
    run(RunStage stage) override;

private:

    void
    threadLoop();

    std::atomic_bool canceled_{false};

    std::vector<std::thread> threadPool_;
    TimePoint startTime_;
    std::condition_variable taskReleased_;
    std::mutex tasksMutex_;
    std::multimap<TimePoint, std::shared_ptr<Task>> tasks_;

    std::unordered_set<std::string> providingRunning_;
    std::unordered_set<std::string> dependingRunning_;
};


#endif //THREADPOOLSCHEDULER_H
