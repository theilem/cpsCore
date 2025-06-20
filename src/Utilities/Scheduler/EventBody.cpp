/*
 * EventBody.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/Scheduler/EventBody.h"

EventBody::EventBody(std::function<void()> b, const std::string& name) :
		body(std::move(b)), period(std::nullopt), eventName(name)
{
	if (eventName.empty())
	{
		eventName = body.target_type().name();
	}
}

EventBody::EventBody(std::function<void()> b, const Duration& p, const std::string& name) :
		body(std::move(b)), period(p), eventName(name)
{
	if (eventName.empty())
	{
		eventName = body.target_type().name();
	}
}
