/*
 * MessageQueuePublisherImpl.h
 *
 *  Created on: Aug 3, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_MESSAGEQUEUEPUBLISHERIMPL_H_
#define UAVAP_CORE_IPC_MESSAGEQUEUEPUBLISHERIMPL_H_
#include <boost/interprocess/ipc/message_queue.hpp>
#include "cpsCore/Utilities/IPC/detail/IPublisherImpl.h"

class MessageQueuePublisherImpl: public IPublisherImpl
{
public:

	MessageQueuePublisherImpl(const std::string& id, unsigned int numOfMessages, std::size_t elementSize);

	~MessageQueuePublisherImpl();

	bool
	publish(const Packet& obj) override;

private:

	boost::interprocess::message_queue messageQueue_;

	std::string id_;

	std::size_t maxPacketSize_;

};

#endif /* UAVAP_CORE_IPC_MESSAGEQUEUEPUBLISHERIMPL_H_ */
