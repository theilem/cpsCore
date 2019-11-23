/*
 * ISubscriptionImpl.h
 *
 *  Created on: Aug 3, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_DETAIL_ISUBSCRIPTIONIMPL_H_
#define UAVAP_CORE_IPC_DETAIL_ISUBSCRIPTIONIMPL_H_

#include <boost/signals2.hpp>
#include "cpsCore/Utilities/Packet.h"


class ISubscriptionImpl
{
public:

	virtual
	~ISubscriptionImpl() = default;

	virtual void
	cancel() = 0;

	virtual void
	start() = 0;

	using OnPacket = boost::signals2::signal<void(const Packet&)>;
	using OnPacketSlot = OnPacket::slot_type;

	virtual boost::signals2::connection
	subscribe(const OnPacketSlot& slot) = 0;

};

#endif /* UAVAP_CORE_IPC_DETAIL_ISUBSCRIPTIONIMPL_H_ */
