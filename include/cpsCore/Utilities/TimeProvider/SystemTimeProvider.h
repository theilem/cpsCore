/*
 * SystemTimeProvider.h
 *
 *  Created on: Jul 19, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_TIMEPROVIDER_SYSTEMTIMEPROVIDER_H_
#define UAVAP_CORE_TIMEPROVIDER_SYSTEMTIMEPROVIDER_H_

#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Utilities/TimeProvider/ITimeProvider.h"

class SystemTimeProvider : public ITimeProvider, public AggregatableObject<>
{
public:

	static constexpr TypeId typeId = "system_time";

	SystemTimeProvider();

	TimePoint
	now() override;

	Duration
	timeSinceStart() override;

	bool
	waitFor(Duration duration, std::condition_variable& interrupt,
			Lock& lock) override;

	bool
	waitUntil(TimePoint timePoint, std::condition_variable& interrupt,
			  Lock& lock) override;

private:

	TimePoint chronoEpoch_;
	TimePoint startTime_;

};

#endif /* UAVAP_CORE_TIMEPROVIDER_SYSTEMTIMEPROVIDER_H_ */
