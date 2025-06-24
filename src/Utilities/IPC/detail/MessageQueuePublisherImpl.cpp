/*
 * MessageQueuePublisherImpl.cpp
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */
#include "cpsCore/Utilities/IPC/detail/MessageQueuePublisherImpl.h"
#include "cpsCore/Logging/CPSLogger.h"

MessageQueuePublisherImpl::MessageQueuePublisherImpl(const std::string& id, unsigned int numOfMessages,
		std::size_t elementSize) :
		messageQueue_(boost::interprocess::create_only, id.c_str(), numOfMessages, elementSize), id_(
				id), maxPacketSize_(elementSize)
{
}

MessageQueuePublisherImpl::~MessageQueuePublisherImpl()
{
	MessageQueuePublisherImpl::publish(Packet("ending"));
	if (boost::interprocess::message_queue::remove(id_.c_str()))
		CPSLOG_DEBUG << id_ << " message queue removed.";
}

bool
MessageQueuePublisherImpl::publish(const Packet& packet)
{
	if (packet.getSize() > maxPacketSize_)
	{
		CPSLOG_ERROR << "Packet to large: " << packet.getSize() << " > " << maxPacketSize_;
		return false;
	}
	unsigned int priority = 1; //TODO Adjust priority?
	if (!messageQueue_.try_send(static_cast<const void*>(packet.getStart()), packet.getSize(), priority))
	{
		CPSLOG_WARN << "Message queue full. Do not send.";
		return false;
	}
	return true;
}
