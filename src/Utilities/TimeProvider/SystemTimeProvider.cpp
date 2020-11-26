/*
 * SystemTimeProvider.cpp
 *
 *  Created on: Jul 19, 2017
 *      Author: mircot
 */
#include "cpsCore/Utilities/TimeProvider/SystemTimeProvider.h"

SystemTimeProvider::SystemTimeProvider() : chronoEpoch_(TimePoint()), startTime_(Clock::now())
{
}

TimePoint
SystemTimeProvider::now()
{
	return Clock::now();
}

bool
SystemTimeProvider::waitFor(Duration duration, std::condition_variable& interrupt,
							Lock& lock)
{
	return interrupt.wait_for(lock, duration) == std::cv_status::timeout;
}

bool
SystemTimeProvider::waitUntil(TimePoint timePoint, std::condition_variable& interrupt,
							  Lock& lock)
{
	return interrupt.wait_until(lock, timePoint) == std::cv_status::timeout;
}

Duration
SystemTimeProvider::timeSinceStart()
{
	return now() - startTime_;
}
