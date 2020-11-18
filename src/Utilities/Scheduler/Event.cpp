/*
 * Event.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/Scheduler/Event.h"
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
Event::isCanceled()
{
	if (auto body = body_.lock())
	{
		return body->isCanceled.load();
	}
	return true;
}
