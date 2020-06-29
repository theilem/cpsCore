/*
 * SharedMemorySubscriptionImpl.h
 *
 *  Created on: Aug 3, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_DETAIL_SHAREDMEMORYSUBSCRIPTIONIMPL_H_
#define UAVAP_CORE_IPC_DETAIL_SHAREDMEMORYSUBSCRIPTIONIMPL_H_

#include <thread>

#include <boost/interprocess/shared_memory_object.hpp>

#include "cpsCore/Utilities/IPC/detail/ISubscriptionImpl.h"

class Packet;

class SharedMemorySubscriptionImpl: public ISubscriptionImpl
{
public:

	explicit SharedMemorySubscriptionImpl(const std::string& id);

	~SharedMemorySubscriptionImpl();

	void
	cancel() override;

	void
	start() override;

	boost::signals2::connection
	subscribe(const OnPacketSlot& slot) override;

private:

	void
	onSharedMemory();

	boost::interprocess::shared_memory_object sharedMem_;

	OnPacket onSharedMem_;

	std::thread listenerThread_;

	std::atomic_bool listenerCanceled_;

	std::string id_;
};
#endif /* UAVAP_CORE_IPC_DETAIL_SHAREDMEMORYSUBSCRIPTIONIMPL_H_ */
