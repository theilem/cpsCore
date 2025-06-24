/*
 * MicroSimulator.h
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_MICROSIMULATOR_H_
#define UAVAP_CORE_SCHEDULER_MICROSIMULATOR_H_

#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>

#include <boost/thread.hpp>

#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Configuration/ConfigurableObject.hpp"
#include "cpsCore/Synchronization/IRunnableObject.h"
#include "cpsCore/Utilities/Scheduler/IScheduler.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulatorParams.h"
#include "cpsCore/Utilities/Time.hpp"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"
#include "cpsCore/Utilities/LockTypes.hpp"

class MicroSimulator
    : public IScheduler,
      public ITimeProvider,
      public IRunnableObject,
      public AggregatableObject<>,
      public ConfigurableObject<MicroSimulatorParams>
{
public:
    static constexpr TypeId typeId = "micro_sim";

    MicroSimulator();
    bool
    run(RunStage stage) override;

    ~MicroSimulator() override;

    Event
    schedule(const std::function<void
                 ()>& task, Duration initialFromNow, const std::string& eventName = "") override;

    Event
    schedule(const std::function<void
                 ()>& task, Duration initialFromNow, Duration period, const std::string& eventName = "") override;

    void
    stop() override;

    void
    setMainThread() override;

    void
    startSchedule() override;

    TimePoint
    now() override;

    Duration
    timeSinceStart() override;

    bool
    waitFor(Duration duration, std::condition_variable& interrupt,
            std::unique_lock<std::mutex>& lock) override;

    bool
    waitUntil(TimePoint timePoint, std::condition_variable& interrupt,
              std::unique_lock<std::mutex>& lock) override;

    int
    simulate(Duration duration);

    int
    simulate();

    void
    stopOnWait();

    void
    releaseWait();

    void
    clearSchedule();

    bool
    isStopped();

    std::mutex&
    getSimMutex();

    void
    setRealTimeFactor(float factor);

private:
    void
    logProgress(const TimePoint end, const Duration duration);

    void
    drawProgressBar(float progress);

    std::multimap<TimePoint, std::shared_ptr<EventBody>> events_;

    TimePoint now_;
    int runs_;
    bool stopOnWait_;
    std::mutex simMutex_;
	std::condition_variable realTimeCondition_;
    float realTimeFactor_;

    Mutex waitCondMutex_;
    std::condition_variable* waitCondition_;
    bool waitReleased_;
    bool stopped_;
};

#endif /* UAVAP_CORE_SCHEDULER_MICROSIMULATOR_H_ */
