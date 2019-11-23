/*
 * EventBody.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/Scheduler/EventBody.h"

EventBody::EventBody(const std::function<void
()>& b) :
		body(b), isCanceled(false), isStarted(false), missedDeadline(false)
{
}

EventBody::EventBody(const std::function<void
()>& b, const Duration& p) :
		body(b), period(p), isCanceled(false), isStarted(false), missedDeadline(false)
{
}
