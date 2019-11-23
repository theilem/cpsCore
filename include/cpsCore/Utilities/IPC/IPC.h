/*
 * IPC.h
 *
 *  Created on: Jul 18, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_IPC_H_
#define UAVAP_CORE_IPC_IPC_H_

#include "cpsCore/Configuration/ConfigurableObject.hpp"
#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Synchronization/IRunnableObject.h"
#include "cpsCore/Utilities/IPC/detail/SharedMemorySubscriptionImpl.h"
#include "cpsCore/Utilities/IPC/Publisher.h"
#include "cpsCore/Utilities/IPC/Subscription.h"
#include "cpsCore/Utilities/Scheduler/IScheduler.h"
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/IPC/IPCOptions.h"
#include "cpsCore/Utilities/IPC/IPCParams.h"
#include "cpsCore/Utilities/LockTypes.hpp"
#include "cpsCore/Utilities/DataPresentation/DataPresentation.h"
#include "cpsCore/Utilities/DataPresentation/BinarySerialization.hpp"

class SignalHandler;

class IPC : public IRunnableObject, public AggregatableObject<IScheduler, DataPresentation,
		SignalHandler>, public ConfigurableObject<IPCParams>
{

public:

	static constexpr TypeId typeId = "ipc";

	IPC();

	~IPC();

	template<typename Type>
	Subscription
	subscribe(const std::string& id, const std::function<void
			(const Type&)>& slot, const IPCOptions& options = IPCOptions());

	Subscription
	subscribeOnPackets(const std::string& id, const std::function<void
			(const Packet&)>& slot, const IPCOptions& options = IPCOptions());

	template<typename Type>
	Publisher<Type>
	publish(const std::string& id, const IPCOptions& options = IPCOptions());

	Publisher<Packet>
	publishPackets(const std::string& id, const IPCOptions& options = IPCOptions());

	bool
	run(RunStage stage) override;

private:

	Subscription
	subscribeOnSharedMemory(const std::string& id, const std::function<void
			(const Packet&)>& slot);

	Subscription
	subscribeOnMessageQueue(const std::string& id, const std::function<void
			(const Packet&)>& slot);

	std::shared_ptr<IPublisherImpl>
	publishOnSharedMemory(const std::string& id, unsigned maxPacketSize);

	std::shared_ptr<IPublisherImpl>
	publishOnMessageQueue(const std::string& id, unsigned maxPacketSize, unsigned numPackets);

	void
	sigintHandler(int sig);

	template<typename Type>
	void
	forwardFromPacket(const Packet& packet, const std::function<void
			(const Type&)>& func) const;

	template<typename Type>
	Packet
	forwardToPacket(const Type& val) const;

	void
	retrySubscriptions();

	std::vector<std::shared_ptr<IPublisherImpl>> publications_;

	Mutex subscribeMutex_;
	std::map<std::string, std::shared_ptr<ISubscriptionImpl>> subscriptions_;

	Mutex retryVectorMutex_;
	std::vector<std::pair<std::function<void
			(const Subscription&)>, std::function<Subscription
			()>>> retryVector_;

};

template<typename Type>
inline Publisher<Type>
IPC::publish(const std::string& id, const IPCOptions& options)
{
	auto packetSize = forwardToPacket(Type()).getSize();
	auto forwarding = std::bind(&IPC::forwardToPacket<Type>, this, std::placeholders::_1);
	if (options.multiTarget)
	{
		return Publisher<Type>(publishOnSharedMemory(id, packetSize), forwarding);
	}
	else
	{
		return Publisher<Type>(publishOnMessageQueue(id, packetSize, params.maxNumPackets()),
							   forwarding);
	}
}

template<typename Type>
inline Subscription
IPC::subscribe(const std::string& id, const std::function<void
		(const Type&)>& slot, const IPCOptions& options)
{
	Subscription result;
	std::function<void(const Packet&)> packetSlot = std::bind(&IPC::forwardFromPacket<Type>, this,
															  std::placeholders::_1, slot);
	if (options.multiTarget)
	{
		result = subscribeOnSharedMemory(id, packetSlot);
	}
	else
	{
		result = subscribeOnMessageQueue(id, packetSlot);
	}

	if (!result.connected() && options.retry)
	{
		LockGuard l(retryVectorMutex_);
		CPSLOG_DEBUG << "Queuing " << id << " for retry";
		//Schedule retry
		if (options.multiTarget)
		{
			retryVector_.push_back(
					std::make_pair(options.retrySuccessCallback,
								   std::bind(&IPC::subscribeOnSharedMemory, this, id, packetSlot)));
		}
		else
		{
			retryVector_.push_back(
					std::make_pair(options.retrySuccessCallback,
								   std::bind(&IPC::subscribeOnMessageQueue, this, id, packetSlot)));
		}
	}

	return result;
}

template<typename Type>
inline void
IPC::forwardFromPacket(const Packet& packet, const std::function<void
		(const Type&)>& func) const
{

	if (auto dp = get<DataPresentation>())
		func(dp->deserialize<Type>(packet));
	else
		func(dp::deserialize<Type>(packet));

}

template<typename Type>
inline Packet
IPC::forwardToPacket(const Type& val) const
{
	if (auto dp = get<DataPresentation>())
		return dp->serialize(val);
	else
		return dp::serialize(val);

}

#endif /* UAVAP_CORE_IPC_IPC_H_ */
