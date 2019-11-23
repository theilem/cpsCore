/*
 * MicroSimulator.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/Scheduler/EventBody.h"
#include <utility>

MicroSimulator::MicroSimulator() :
		now_(), runs_(0), stopOnWait_(false), waitCondition_(
		nullptr), waitReleased_(false), stopped_(false)
{
}

Event
MicroSimulator::schedule(const std::function<void
		()>& task, Duration initialFromNow)
{
	auto body = std::make_shared<EventBody>(task);
	events_.insert(std::make_pair(now_ + initialFromNow, body));
	return Event(body);
}

Event
MicroSimulator::schedule(const std::function<void
		()>& task, Duration initialFromNow, Duration period)
{
	auto body = std::make_shared<EventBody>(task, period);
	events_.insert(std::make_pair(now_ + initialFromNow, body));
	return Event(body);
}

void
MicroSimulator::stop()
{
	CPSLOG_DEBUG << "Stop MicroSim called";
	stopped_ = true;
}

int
MicroSimulator::simulate(Duration duration)
{
	runs_ = 0;
	TimePoint endSim = now_ + duration;
	while (now_ <= endSim)
	{
		if (events_.empty())
		{
			break;
		}

		if (events_.begin()->first > endSim)
		{
			break;
		}

		if (params.realTimeFactor() > 0)
		{
			std::this_thread::sleep_for((events_.begin()->first - now_) * params.realTimeFactor());
		}

		if (stopped_)
		{
			CPSLOG_DEBUG << "MicroSim Scheduler was stopped.";
			return runs_;
		}

		now_ = events_.begin()->first;
		events_.begin()->second->body();
		if (events_.begin()->second->period)
		{
			schedule(events_.begin()->second->body, *events_.begin()->second->period,
					 *events_.begin()->second->period);
		}
		++runs_;
		events_.erase(events_.begin());
	}

	now_ = endSim;
	return runs_;
}

void
MicroSimulator::stopOnWait()
{
	stopOnWait_ = true;
}

void
MicroSimulator::setMainThread()
{
}

void
MicroSimulator::startSchedule()
{
}

void
MicroSimulator::releaseWait()
{
	std::unique_lock<std::mutex> lock(waitCondMutex_);
	if (!waitCondition_)
	{
		CPSLOG_WARN << "Nobody waiting on release";
		return;
	}
	waitReleased_ = true;
	waitCondition_->notify_all();
}

TimePoint
MicroSimulator::now()
{
	return now_;
}

bool
MicroSimulator::waitFor(Duration duration, std::condition_variable& interrupt,
						std::unique_lock<std::mutex>& lock)
{
	if (stopOnWait_)
	{
		waitCondition_ = &interrupt;
		waitCondition_->wait(lock);
	}
	bool waitReleased = waitReleased_;
	waitReleased_ = false;
	if (waitReleased)
		now_ += duration;
	return waitReleased;
}

bool
MicroSimulator::waitUntil(TimePoint timePoint, std::condition_variable& interrupt,
						  std::unique_lock<std::mutex>& lock)
{
	if (stopOnWait_)
	{

		waitCondition_ = &interrupt;
		waitCondition_->wait(lock);
	}
	bool waitReleased = waitReleased_;
	waitReleased_ = false;
	if (waitReleased)
		now_ = timePoint;
	return waitReleased;
}

void
MicroSimulator::clearSchedule()
{
	events_.clear();
}

bool
MicroSimulator::isStopped()
{
	return stopped_;
}
