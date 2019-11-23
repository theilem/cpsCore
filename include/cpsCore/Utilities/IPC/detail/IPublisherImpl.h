/*
 * IPublisherImpl.h
 *
 *  Created on: Aug 2, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_IPUBLISHERIMPL_H_
#define UAVAP_CORE_IPC_IPUBLISHERIMPL_H_

#include "cpsCore/Utilities/Packet.h"

class IPublisherImpl
{
public:

	virtual
	~IPublisherImpl() = default;

	virtual void
	publish(const Packet& obj) = 0;

};

#endif /* UAVAP_CORE_IPC_IPUBLISHERIMPL_H_ */
