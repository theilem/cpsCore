/*
 * SharedMemoryPublisherImpl.cpp
 *
 *  Created on: Jul 15, 2019
 *      Author: mirco
 */
#include <thread>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread/lock_types.hpp>

#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/IPC/detail/SharedMemoryPublisherImpl.h"
#include "cpsCore/Utilities/Time.hpp"
#include "cpsCore/Utilities/IPC/detail/MessageObject.h"


SharedMemoryPublisherImpl::SharedMemoryPublisherImpl(const std::string& id, std::size_t init) :
		maxPacketSize_(init)
{
	using namespace boost::interprocess;

	bool existing = false;
	try
	{
		sharedMem_ = shared_memory_object(open_only, id.c_str(), read_write);
		existing = true;
		CPSLOG_WARN << "Shared memory object " << id << " aready exists. Using existing.";
	} catch (interprocess_exception&)
	{
		sharedMem_ = shared_memory_object(create_only, id.c_str(), read_write);
		sharedMem_.truncate(sizeof(MessageObjectHeader) + maxPacketSize_);
	}

	mapped_region region(sharedMem_, read_write);

	if (!existing)
	{
		MessageObjectHeader header;
		header.maxPacketSize = maxPacketSize_;
		header.packetSize = 0;
		header.active = true;

		memcpy(region.get_address(), &header, sizeof(header));
	}
	else
	{
		// if existing just set values, do not copy new conditions and mutexes so that waiting processes don't miss out.
		auto header = static_cast<MessageObjectHeader*>(region.get_address());
		header->maxPacketSize = maxPacketSize_;
		header->packetSize = 0;
		header->active = true;
	}

}

bool
SharedMemoryPublisherImpl::publish(const Packet& packet)
{
	using namespace boost::interprocess;

	auto size = packet.getSize();

	if (size > maxPacketSize_)
	{
		CPSLOG_ERROR << "Packet to big, cannot copy to shm.";
		return false;
	}

	mapped_region region(sharedMem_, read_write);
	MessageObjectHeader* message = static_cast<MessageObjectHeader*>(region.get_address());

	boost::unique_lock<boost::interprocess::interprocess_sharable_mutex> lock(message->mtx);

	message->packetSize = size;
	memcpy((uint8_t*) region.get_address() + sizeof(MessageObjectHeader), packet.getStart(), size);

	message->cnd.notify_all();
	return true;
}

SharedMemoryPublisherImpl::~SharedMemoryPublisherImpl()
{
	teardown();
	std::this_thread::sleep_for(Milliseconds(10)); //allow subscribers to disconnect

	std::string name = sharedMem_.get_name();
	if (sharedMem_.remove(name.c_str()))
		CPSLOG_DEBUG << name << " shared memory removed.";
}

void
SharedMemoryPublisherImpl::teardown()
{
	boost::interprocess::mapped_region region(sharedMem_, boost::interprocess::read_write);
	MessageObjectHeader* message = static_cast<MessageObjectHeader*>(region.get_address());

	boost::unique_lock<boost::interprocess::interprocess_sharable_mutex> lock(message->mtx);

	message->active = false;
	message->cnd.notify_all();

	lock.unlock();
}
