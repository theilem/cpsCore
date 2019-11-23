/**
 * @file MessageObject.h
 * @author Mirco Theile, mirco.theile@tum.de
 * @date [DD.MM.YYYY] 18.07.2017
 */

#ifndef UAVAP_CORE_IPC_MESSAGEOBJECT_H_
#define UAVAP_CORE_IPC_MESSAGEOBJECT_H_
#include <boost/interprocess/sync/interprocess_condition_any.hpp>
#include <boost/interprocess/sync/interprocess_sharable_mutex.hpp>

struct MessageObjectHeader
{
	boost::interprocess::interprocess_condition_any cnd;
	boost::interprocess::interprocess_sharable_mutex mtx;

	bool active;

	std::size_t maxPacketSize;
	std::size_t packetSize;

	MessageObjectHeader() :
			active(true), maxPacketSize(0), packetSize(0)
	{
	}
};

template<class Object>
struct MessageObject: public Object
{
	MessageObjectHeader header;

	MessageObject() :
			Object()
	{
	}

};

#endif /* UAVAP_CORE_IPC_MESSAGEOBJECT_H_ */
