/*
 * IScheduler.h
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_ISCHEDULER_H_
#define UAVAP_CORE_SCHEDULER_ISCHEDULER_H_

#include "cpsCore/Utilities/Scheduler/Event.h"
#include "cpsCore/Utilities/Time.hpp"
#include <functional>

class IScheduler
{
public:

	static constexpr auto typeId = "scheduler";

	virtual
	~IScheduler()
	= default;

	using TaskHandle = std::function<void()>;

	virtual Event
	schedule(const TaskHandle& task, Duration initialFromNow, const std::string& eventName = "") = 0;

	virtual Event
	schedule(const TaskHandle& task, Duration initialFromNow, Duration period, const std::string& eventName = "") = 0;

	virtual void
	stop() = 0;

	virtual void
	setMainThread() = 0;

	virtual void
	startSchedule() = 0;
};

#endif /* UAVAP_CORE_SCHEDULER_ISCHEDULER_H_ */
