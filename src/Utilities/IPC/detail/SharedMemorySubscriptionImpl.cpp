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
		listenerCanceled_(true), id_(id)
{
	connected_ = connect();
}

boost::signals2::connection
SharedMemorySubscriptionImpl::subscribe(const OnPacketSlot& slot)
{
	return onSharedMem_.connect(slot);
}

void
SharedMemorySubscriptionImpl::cancel()
{
	if (listenerCanceled_.load())
	{
		return;
	}
	CPSLOG_DEBUG << "Calling cancel on shm sub";
	listenerCanceled_.store(true);
	listenerThread_.join();
	CPSLOG_DEBUG << "Joined Listener thread";
}

SharedMemorySubscriptionImpl::~SharedMemorySubscriptionImpl()
{
	cancel();
}

void
SharedMemorySubscriptionImpl::start()
{
	listenerCanceled_.store(false);
	if (listenerThread_.joinable())
	{
		listenerThread_.join();
	}
	listenerThread_ = std::thread([this]
								  { onSharedMemory(); });
}

void
SharedMemorySubscriptionImpl::onSharedMemory()
{
	using namespace boost::interprocess;
	mapped_region region(sharedMem_, read_write);
	auto* message = static_cast<MessageObjectHeader*>(region.get_address());
	Packet packet;
	packet.getBuffer().reserve(message->maxPacketSize);
	const void* packetStart = static_cast<void*>(static_cast<char*>(region.get_address()) + sizeof(
			MessageObjectHeader));

	sharable_lock lock(message->mtx);
	while (!listenerCanceled_.load())
	{
		auto timeout = boost::get_system_time() + boost::posix_time::milliseconds(10);
		if (!message->cnd.timed_wait(lock, timeout))
		{
			continue;
		}
		if (!message->active)
		{
			CPSLOG_DEBUG << "Shm object " << id_ << " inactive. End subscription.";
			break;
		}
		packet.getBuffer().resize(message->packetSize);
		memcpy(packet.getStartAddress(), packetStart, message->packetSize);
		onSharedMem_(packet);
	}
	listenerCanceled_.store(true);
	connected_ = false;
}

bool
SharedMemorySubscriptionImpl::connected() const
{
	return connected_;
}

bool
SharedMemorySubscriptionImpl::connect()
{
	try
	{
		sharedMem_ = boost::interprocess::shared_memory_object(boost::interprocess::open_only, id_.c_str(),
															   boost::interprocess::read_write);
		connected_ = true;
		return true;
	}
	catch (boost::interprocess::interprocess_exception&)
	{
		CPSLOG_DEBUG << "Shared memory object " << id_ << " does not exist. Cannot subscribe.";
		return false;
	}
}
