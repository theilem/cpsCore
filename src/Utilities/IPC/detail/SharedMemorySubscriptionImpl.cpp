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
        false), id_(id)
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
    CPSLOG_DEBUG << "Calling cancel on shm sub";
    listenerCanceled_.store(true);
    listenerThread_.join();
    CPSLOG_DEBUG << "Joined Listener thread";
}

SharedMemorySubscriptionImpl::~SharedMemorySubscriptionImpl()
{
    if (!listenerCanceled_.load())
    {
        cancel();
    }
}

void
SharedMemorySubscriptionImpl::start()
{
    listenerThread_ = std::thread(
        std::bind(&SharedMemorySubscriptionImpl::onSharedMemory, this));
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
            return;
        }
        packet.getBuffer().resize(message->packetSize);
        memcpy(packet.getStartAddress(), packetStart, message->packetSize);
        onSharedMem_(packet);
    }
}
