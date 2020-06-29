/*
 * ITimeProvider.h
 *
 *  Created on: Jul 19, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_TIMEPROVIDER_ITIMEPROVIDER_H_
#define UAVAP_CORE_TIMEPROVIDER_ITIMEPROVIDER_H_
#include <condition_variable>

#include "cpsCore/Utilities/Time.hpp"
#include "cpsCore/Utilities/LockTypes.hpp"

class ITimeProvider
{
public:

	static constexpr auto typeId = "time_provider";

	virtual
	~ITimeProvider() = default;

	virtual TimePoint
	now() = 0;

	virtual bool
	waitFor(Duration duration, std::condition_variable& interrupt,
			Lock& lock) = 0;

	virtual	bool
	waitUntil(TimePoint timePoint, std::condition_variable& interrupt,
			  Lock& lock) = 0;

private:

};

#endif /* UAVAP_CORE_TIMEPROVIDER_ITIMEPROVIDER_H_ */
