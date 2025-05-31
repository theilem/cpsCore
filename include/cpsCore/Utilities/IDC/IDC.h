/*
 * IDC.h
 *
 *  Created on: Jul 28, 2018
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IDC_IDC_H_
#define UAVAP_CORE_IDC_IDC_H_
#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Synchronization/IRunnableObject.h"
#include "cpsCore/Utilities/IDC/IDCSender.h"

#include <boost/signals2.hpp>

#include "NetworkLayer/INetworkLayer.h"
#include "TransportLayer/ITransportLayer.h"

class Packet;

class IDC;

class IDC: public AggregatableObject<INetworkLayer, ITransportLayer, IDC>, public IRunnableObject
{

public:

	static constexpr auto typeId = "idc";

	bool
	run(RunStage stage) override;

	bool
	sendPacket(const std::string& id, const Packet& packet, bool ack = false);

	using OnPacket = boost::signals2::signal<void(const Packet&)>;

	boost::signals2::connection
	subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle);

	IDCSender
	createSender(const std::string& id);



};

#endif /* UAVAP_CORE_IDC_IDC_H_ */
