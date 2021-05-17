/*
 * EventBody.h
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_EVENTBODY_H_
#define UAVAP_CORE_SCHEDULER_EVENTBODY_H_

#include <functional>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <optional>

#include "cpsCore/Utilities/Time.hpp"

struct EventBody
{

	std::function<void
			()> body;
	std::optional<Duration> period;
	std::atomic_bool isCanceled;
	std::atomic_bool isStarted;
	std::atomic_bool missedDeadline;

	std::mutex executionMutex;
	std::thread periodicThread;
	std::condition_variable intervalCondition;

	explicit EventBody(std::function<void
			()> b);

	EventBody(std::function<void
			()> b, const Duration& p);

};

#endif /* UAVAP_CORE_SCHEDULER_EVENTBODY_H_ */
