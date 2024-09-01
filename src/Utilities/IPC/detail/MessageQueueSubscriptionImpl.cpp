/*
 * MessageQueueSubscriptionImpl.cpp
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */
#include <boost/thread/thread_time.hpp>
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/Time.hpp"
#include "cpsCore/Utilities/IPC/detail/MessageQueueSubscriptionImpl.h"

MessageQueueSubscriptionImpl::MessageQueueSubscriptionImpl(const std::string& id,
		std::size_t maxPacketSize) :
		messageQueue_(boost::interprocess::open_only, id.c_str()), listenerCanceled_(false), maxPacketSize_(
				maxPacketSize), id_(id)
{

}

MessageQueueSubscriptionImpl::~MessageQueueSubscriptionImpl()
{
	if (!listenerCanceled_.load())
	{
		cancel();
	}
}

void
MessageQueueSubscriptionImpl::cancel()
{
	listenerCanceled_.store(true);
	listenerThread_.join();
}

void
MessageQueueSubscriptionImpl::start()
{
	listenerThread_ = std::thread(std::bind(&MessageQueueSubscriptionImpl::onMessageQueue, this));
}

boost::signals2::connection
MessageQueueSubscriptionImpl::subscribe(const OnPacketSlot& slot)
{
	return onMessageQueue_.connect(slot);
}

void
MessageQueueSubscriptionImpl::onMessageQueue()
{
	using namespace boost::interprocess;

	while (true)
	{
		if (listenerCanceled_.load())
		{
			return;
		}
		std::string packet;
		packet.resize(maxPacketSize_);
		message_queue::size_type size;
		unsigned int priority;

		auto timeout = boost::get_system_time() + boost::posix_time::milliseconds(100);
		if (!messageQueue_.timed_receive(static_cast<void*>(&packet[0]), maxPacketSize_, size,
				priority, timeout))
		{
			continue;
		}
		packet.resize(size);
		packet += ' ';
		onMessageQueue_(Packet(packet));
	}
}
