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
		listenerCanceled_(true), maxPacketSize_(
		maxPacketSize), id_(id)
{
	connect();
}

MessageQueueSubscriptionImpl::~MessageQueueSubscriptionImpl()
{
	cancel();
}

void
MessageQueueSubscriptionImpl::cancel()
{
	if (listenerCanceled_.load())
	{
		return;
	}
	listenerCanceled_.store(true);
	listenerThread_.join();
}

void
MessageQueueSubscriptionImpl::start()
{
	listenerCanceled_.store(false);
	if (listenerThread_.joinable())
	{
		listenerThread_.join();
	}
	listenerThread_ = std::thread([this]
								  { onMessageQueue(); });
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
		if (!messageQueue_->timed_receive(static_cast<void*>(&packet[0]), maxPacketSize_, size,
										  priority, timeout))
		{
			continue;
		}
		packet.resize(size);
		packet += ' ';
		onMessageQueue_(Packet(packet));
	}
}

bool
MessageQueueSubscriptionImpl::connect()
{
	try
	{
		messageQueue_ = std::make_unique<boost::interprocess::message_queue>(boost::interprocess::open_only,
																			 id_.c_str());
		return true;
	} catch (const boost::interprocess::interprocess_exception& e)
	{
		CPSLOG_WARN << "Could not connect to message queue: " << e.what();
		messageQueue_.reset();
		return false;
	}
}

bool
MessageQueueSubscriptionImpl::connected() const
{
	return messageQueue_ != nullptr;
}
