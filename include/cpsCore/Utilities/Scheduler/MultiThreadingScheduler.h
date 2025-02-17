/*
 * MultiThreadedScheduler.h
 *
 *  Created on: Jul 19, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_MULTITHREADINGSCHEDULER_H_
#define UAVAP_CORE_SCHEDULER_MULTITHREADINGSCHEDULER_H_

#include <thread>

#include "cpsCore/cps_object"
#include "cpsCore/Utilities/Scheduler/EventBody.h"
#include "cpsCore/Utilities/Scheduler/IScheduler.h"
#include "cpsCore/Utilities/Scheduler/MultiThreadingSchedulerParams.h"

class SignalHandler;

class ITimeProvider;

class MultiThreadingScheduler : public IScheduler,
								public AggregatableObject<ITimeProvider, SignalHandler>,
								public ConfigurableObject<MultiThreadingSchedulerParams>,
								public IRunnableObject
{
public:

	static constexpr TypeId typeId = "thread";

	/**
	 * @brief
	 */
	MultiThreadingScheduler();

	~MultiThreadingScheduler() override;

	Event
	schedule(const std::function<void()>& task, Duration initialFromNow, const std::string& eventName = "") override;

	Event
	schedule(const std::function<void
			()>& task, Duration initialFromNow, Duration period, const std::string& eventName = "") override;

	void
	stop() override;

	bool
	run(RunStage stage) override;

	void
	setMainThread() override;

	void
	startSchedule() override;

private:

	void
	runSchedule();

	using EventMap = std::multimap<Duration, std::shared_ptr<EventBody> >;

	EventMap::value_type
	createSchedule(Duration start, std::shared_ptr<EventBody> body);

	void
	periodicTask(std::shared_ptr<EventBody> body);

	void
	handleMissedDeadline(std::shared_ptr<EventBody> body);

	EventMap events_;

	std::vector<std::shared_ptr<EventBody>> nonPeriodicEvents_;

	std::thread invokerThread_;

	std::condition_variable wakeupCondition_;
	std::mutex eventsMutex_;

	std::atomic_bool started_;
	TimePoint startingTime_;

	bool mainThread_;

	sched_param schedulingParams_;
};

#endif /* UAVAP_CORE_SCHEDULER_MULTITHREADINGSCHEDULER_H_ */
