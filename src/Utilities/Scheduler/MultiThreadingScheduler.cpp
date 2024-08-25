/*
 * MultiThreadingScheduler.cpp
 *
 *  Created on: Jul 19, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/LockTypes.hpp"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"
#include <utility>

MultiThreadingScheduler::MultiThreadingScheduler() :
		started_(false), mainThread_(false)
{
}

MultiThreadingScheduler::~MultiThreadingScheduler()
{
	if (started_)
		stop();

	CPSLOG_TRACE << "Scheduler destroyed.";
}

Event
MultiThreadingScheduler::schedule(const std::function<void
		()>& task, Duration initialFromNow)
{
	auto body = std::make_shared<EventBody>(task);
	auto element = createSchedule(initialFromNow, body);

	std::unique_lock<std::mutex> lock(eventsMutex_);
	events_.insert(element);
	wakeupCondition_.notify_all();
	return Event(body);
}

Event
MultiThreadingScheduler::schedule(const std::function<void
		()>& task, Duration initialFromNow, Duration period)
{
	auto body = std::make_shared<EventBody>(task, period);
	auto element = createSchedule(initialFromNow, body);

	std::unique_lock<std::mutex> lock(eventsMutex_);
	events_.insert(element);
	wakeupCondition_.notify_all();
	return Event(body);
}

void
MultiThreadingScheduler::stop()
{
	std::unique_lock<std::mutex> lock(eventsMutex_);
	started_ = false;

	for (auto& event : events_)
	{
		Lock l(event.second->executionMutex);
		event.second->isCanceled.store(true);
		event.second->intervalCondition.notify_all();
		l.unlock();
	}
	for (auto& event : events_)
	{
		if (event.second->periodicThread.joinable())
		{
			CPSLOG_TRACE << "Joining " << event.second->body.target_type().name();
			CPSLogger::instance()->flush();
			event.second->periodicThread.join();
			CPSLOG_TRACE << "Joined " << event.second->body.target_type().name();
			CPSLogger::instance()->flush();
		}
		else
		{
			CPSLOG_WARN << event.second->body.target_type().name() << " not joinable";
		}
	}

	for (auto event : nonPeriodicEvents_)
	{
		if (event->periodicThread.joinable())
		{
			CPSLOG_TRACE << "Joining non-periodic " << event->body.target_type().name();
			CPSLogger::instance()->flush();
			event->periodicThread.join();
			CPSLOG_TRACE << "Joined non-periodic " << event->body.target_type().name();
			CPSLogger::instance()->flush();
		}
	}


	events_.clear();
	wakeupCondition_.notify_all();
	lock.unlock();


	if (!mainThread_)
		invokerThread_.join();
}

bool
MultiThreadingScheduler::run(RunStage stage)
{
	switch (stage)
	{
		case RunStage::INIT:
		{
			if (!checkIsSet<ITimeProvider>())
			{
				CPSLOG_ERROR << "MultiThreadingScheduler deps missing.";
				return true;
			}
			schedulingParams_.sched_priority = params.priority();
			break;
		}
		case RunStage::NORMAL:
		{

			if (auto sigHand = get<SignalHandler>())
				sigHand->subscribeOnExit(std::bind(&MultiThreadingScheduler::stop, this));
			break;
		}
		case RunStage::FINAL:
		{
			started_ = true;
			if (!mainThread_)
			{
				invokerThread_ = std::thread(std::bind(&MultiThreadingScheduler::runSchedule, this));

				if (params.priority() != 20)
					pthread_setschedparam(invokerThread_.native_handle(), SCHED_FIFO,
										  &schedulingParams_);
			}
			else
			{
				if (params.priority() != 20)
					pthread_setschedparam(pthread_self(), SCHED_FIFO, &schedulingParams_);
			}

			break;
		}
		default:
			break;
	}
	return false;
}

void
MultiThreadingScheduler::runSchedule()
{
	auto timeProvider = get<ITimeProvider>();
	if (!timeProvider)
	{
		CPSLOG_ERROR << "Time Provider missing in Scheduler. Abort.";
		return;
	}

	CPSLOG_TRACE << "Starting Scheduling";

	std::unique_lock<std::mutex> lock(eventsMutex_);

	startingTime_ = timeProvider->now();
	for (;;)
	{
		TimePoint now = timeProvider->now();
		while (!events_.empty())
		{
			if (startingTime_ + events_.begin()->first > now)
				break;
			CPSLOG_TRACE << "Executing task";
			auto it = events_.begin();

			auto eventBody = it->second;

			if (eventBody->period)
			{
				//Is a periodic task
				//Check if it is already running
				if (eventBody->isStarted.load())
				{
					//Thread is already started
					std::unique_lock<std::mutex> lock(eventBody->executionMutex, std::try_to_lock);
					if (lock)
					{
						//The task is waiting at the condition variable
						eventBody->intervalCondition.notify_one(); //Only one should be waiting on it
						lock.unlock();
					}
					else
					{
						//Task didn't finish execution yet
						handleMissedDeadline(eventBody);
						continue;
					}
					//Check if isCanceled
					if (eventBody->isCanceled.load())
					{
						nonPeriodicEvents_.push_back(eventBody);
						events_.erase(events_.begin());
						continue;
					}
				}
				else
				{
					if (!eventBody->isCanceled.load())
					{
						//Thread not started yet
						eventBody->periodicThread = std::thread(
								std::bind(&MultiThreadingScheduler::periodicTask, this,
										  eventBody));
						if (params.priority() != 20)
							if (int r = pthread_setschedparam(
									eventBody->periodicThread.native_handle(),
									SCHED_FIFO, &schedulingParams_))
							{
								CPSLOG_DEBUG << "Cannot set sched params: " << r;
							}
					}
				}
				if (!eventBody->isCanceled.load())
				{
					//Reschedule Task
					auto element = std::make_pair(it->first + *eventBody->period, eventBody);
					events_.insert(element);
				}
			}
			else
			{
				//Not a periodic thread
				if (!eventBody->isCanceled.load())
				{
					eventBody->periodicThread = std::thread(it->second->body);
					nonPeriodicEvents_.push_back(eventBody);
//					std::thread(it->second->body).detach();
				}
			}
			//Remove current schedule from events as it was handeled
			events_.erase(events_.begin());
		}
		if (!events_.empty())
		{
			timeProvider->waitUntil(startingTime_ + events_.begin()->first, wakeupCondition_, lock);
		}
		else
		{
			wakeupCondition_.wait(lock);
		}
		if (!started_)
		{
			CPSLOG_DEBUG << "Scheduler was stopped.";
			return;
		}
	}
}

MultiThreadingScheduler::EventMap::value_type
MultiThreadingScheduler::createSchedule(Duration start, std::shared_ptr<EventBody> body)
{
	auto tp = get<ITimeProvider>();
	if (!tp)
	{
		CPSLOG_ERROR << "TimeProvider missing. Cannot Schedule.";
		return std::make_pair(start, body);
	}

	Duration fromStart = start;

	if (started_)
	{
		auto now = tp->now();
		fromStart += now - startingTime_;
	}

	return std::make_pair(fromStart, body);
}

void
MultiThreadingScheduler::periodicTask(std::shared_ptr<EventBody> body)
{
	body->isStarted.store(true);
	std::unique_lock<std::mutex> lock(body->executionMutex);
	while (!body->isCanceled.load())
	{
		body->body();
		body->intervalCondition.wait(lock);
	}
	body->isStarted.store(false);
}

void
MultiThreadingScheduler::setMainThread()
{
	mainThread_ = true;
}

void
MultiThreadingScheduler::startSchedule()
{
	if (!mainThread_)
	{
		CPSLOG_ERROR
				<< "Scheduler not configured to be main thread. Invoker thread probably running.";
		return;
	}
	runSchedule();
}

void
MultiThreadingScheduler::handleMissedDeadline(std::shared_ptr<EventBody> body)
{
	CPSLOG_WARN << body->body.target_type().name() << " Missed Deadline. Waiting...";
	//Reschedule Task
	auto element = std::make_pair(events_.begin()->first + *body->period, body);
	events_.insert(element);
	events_.erase(events_.begin());
}
