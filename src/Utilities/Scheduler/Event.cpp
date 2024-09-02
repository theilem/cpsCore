/*
 * Event.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/Scheduler/Event.h"

#include <cpsCore/Logging/CPSLogger.h>

#include "cpsCore/Utilities/Scheduler/EventBody.h"

Event::Event()
{
}

Event::Event(std::shared_ptr<EventBody> body) :
		body_(body)
{
}

void
Event::cancel()
{
	if (auto body = body_.lock())
	{
		body->isCanceled.store(true);
	}
}

bool
Event::isCanceled() const
{
	if (auto body = body_.lock())
	{
		return body->isCanceled.load();
	}
	return true;
}

void
Event::changePeriod(const Duration& period)
{
	if (auto body = body_.lock())
	{
		if (body->period == std::nullopt)
		{
			CPSLOG_ERROR << "Cannot change period because period was never set.";
			return;
		}
		body->period = period;
		return;
	}
	CPSLOG_ERROR << "Cannot change period because body is not set.";
}
