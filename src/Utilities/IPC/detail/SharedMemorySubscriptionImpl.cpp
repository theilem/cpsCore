/*
 * SharedMemorySubscriptionImpl.cpp
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "cpsCore/Utilities/IPC/detail/MessageObject.h"
#include "cpsCore/Utilities/IPC/detail/SharedMemorySubscriptionImpl.h"
#include "cpsCore/Logging/CPSLogger.h"



SharedMemorySubscriptionImpl::SharedMemorySubscriptionImpl(const std::string& id) :
		sharedMem_(boost::interprocess::open_only, id.c_str(), boost::interprocess::read_write), listenerCanceled_(
				false)
{
}

boost::signals2::connection
SharedMemorySubscriptionImpl::subscribe(const OnPacketSlot& slot)
{
	return onSharedMem_.connect(slot);
}

void
SharedMemorySubscriptionImpl::cancel()
{
	listenerCanceled_.store(true);
	listenerThread_.join();
}

SharedMemorySubscriptionImpl::~SharedMemorySubscriptionImpl()
{
	if (!listenerCanceled_.load())
	{
		cancel();
	}
//	std::this_thread::sleep_for(std::chrono::milliseconds(100)); //Wait for timeout of condition to stop subscription

//	std::string name = sharedMem_.get_name();
//	if (sharedMem_.remove(name.c_str()))
//		APLOG_DEBUG << name << " shared memory removed.";
}

void
SharedMemorySubscriptionImpl::start()
{
	listenerThread_ = std::thread(
			std::bind(&SharedMemorySubscriptionImpl::onSharedMemory, this));
//	listenerThread_.detach();
}

void
SharedMemorySubscriptionImpl::onSharedMemory()
{
	using namespace boost::interprocess;
	mapped_region region(sharedMem_, read_write);
	MessageObjectHeader* message = static_cast<MessageObjectHeader*>(region.get_address());
	Packet packet;
	packet.getBuffer().reserve(message->maxPacketSize);
	void* packetStart = static_cast<void*>((uint8_t*)region.get_address() + sizeof(MessageObjectHeader));

	boost::interprocess::sharable_lock<boost::interprocess::interprocess_sharable_mutex> lock(
			message->mtx);
	for (;;)
	{
		if (listenerCanceled_.load())
		{
			return;
		}

		auto timeout = boost::get_system_time() + boost::posix_time::milliseconds(10);
		if (!message->cnd.timed_wait(lock, timeout))
		{
			continue;
		}
		if (!message->active)
		{
			CPSLOG_DEBUG << "Shm object inactive. End subscription.";
			return;
		}
		packet.getBuffer().resize(message->packetSize);
		memcpy(packet.getStartAddress(), packetStart, message->packetSize);
		onSharedMem_(packet);
	}
}
