/*
 * SharedMemoryPublisherImpl.h
 *
 *  Created on: Aug 2, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_SHAREDMEMORYPUBLISHERIMPL_H_
#define UAVAP_CORE_IPC_SHAREDMEMORYPUBLISHERIMPL_H_

#include <boost/interprocess/shared_memory_object.hpp>

#include "cpsCore/Utilities/IPC/detail/IPublisherImpl.h"

class SharedMemoryPublisherImpl: public IPublisherImpl
{
public:

	SharedMemoryPublisherImpl(const std::string& id, std::size_t maxPacketSize);

	~SharedMemoryPublisherImpl() override;

	bool
	publish(const Packet& obj) override;

private:

	void
	teardown();

	boost::interprocess::shared_memory_object sharedMem_;
	std::size_t maxPacketSize_;
	bool owner_;

};

#endif /* UAVAP_CORE_IPC_SHAREDMEMORYPUBLISHERIMPL_H_ */
