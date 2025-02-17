/*
 * MessageQueueSubscriptionImpl.h
 *
 *  Created on: Aug 3, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_DETAIL_MESSAGEQUEUESUBSCRIPTIONIMPL_H_
#define UAVAP_CORE_IPC_DETAIL_MESSAGEQUEUESUBSCRIPTIONIMPL_H_
#include <thread>
#include <atomic>

#include <boost/interprocess/ipc/message_queue.hpp>

#include "cpsCore/Utilities/IPC/detail/ISubscriptionImpl.h"

class Packet;

class MessageQueueSubscriptionImpl: public ISubscriptionImpl
{
public:

	MessageQueueSubscriptionImpl(const std::string& id, std::size_t maxPacketSize);

	~MessageQueueSubscriptionImpl() override;

	void
	cancel() override;

	void
	start() override;

	bool
	connect() override;

	bool
	connected() const override;


	boost::signals2::connection
	subscribe(const OnPacketSlot& slot) override;

private:

	void
	onMessageQueue();

	std::unique_ptr<boost::interprocess::message_queue> messageQueue_;

	OnPacket onMessageQueue_;

	std::thread listenerThread_;
	std::atomic_bool listenerCanceled_;

	bool connected_;

	std::size_t maxPacketSize_;

	std::string id_;
};

#endif /* UAVAP_CORE_IPC_DETAIL_MESSAGEQUEUESUBSCRIPTIONIMPL_H_ */
