/*
 * EventBody.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/Scheduler/EventBody.h"

EventBody::EventBody(std::function<void
()>  b) :
		body(std::move(b)), isCanceled(false), isStarted(false), missedDeadline(false)
{
}

EventBody::EventBody(std::function<void
()>  b, const Duration& p) :
		body(std::move(b)), period(p), isCanceled(false), isStarted(false), missedDeadline(false)
{
}
