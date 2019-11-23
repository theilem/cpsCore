/*
 * Event.h
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_SCHEDULER_EVENT_H_
#define UAVAP_CORE_SCHEDULER_EVENT_H_
#include <memory>

struct EventBody;

class Event
{
public:

	Event();

	Event(std::weak_ptr<EventBody> body);

	void
	cancel();

	bool
	isCancled();

private:

	std::weak_ptr<EventBody> body_;

};

#endif /* UAVAP_CORE_SCHEDULER_EVENT_H_ */
