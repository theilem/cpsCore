/*
 * Event.h
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_EVENT_H_
#define UAVAP_CORE_SCHEDULER_EVENT_H_

#include <memory>
#include <cpsCore/Utilities/Time.hpp>

struct EventBody;

class Event
{
public:

	Event();

	explicit Event(std::shared_ptr<EventBody> body);

	void
	cancel();

	bool
	isCanceled() const;

	void
	changePeriod(const Duration& period);

private:

	std::weak_ptr<EventBody> body_;

};

#endif /* UAVAP_CORE_SCHEDULER_EVENT_H_ */
