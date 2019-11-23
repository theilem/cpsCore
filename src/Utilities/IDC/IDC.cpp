/*
 * IDC.cpp
 *
 *  Created on: Jul 28, 2018
 *      Author: mircot
 */

#include "cpsCore/Utilities/IDC/IDC.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/INetworkLayer.h"
#include "cpsCore/Utilities/IDC/TransportLayer/ITransportLayer.h"
#include "cpsCore/Logging/CPSLogger.h"

bool
IDC::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!isSet<IDC>())
		{
			CPSLOG_ERROR << "Something went really wrong";
			return true;
		}
		if (!isSet<ITransportLayer>())
		{
			if (!isSet<INetworkLayer>())
			{
				CPSLOG_ERROR << "Transport and Network layer missing. IDC needs one.";
				return true;
			}
		}
		break;
	}
	default:
		break;
	}
	return false;
}

bool
IDC::sendPacket(const std::string& id, const Packet& packet, bool ack)
{
	auto transport = get<ITransportLayer>();

	if (!transport)
	{
		auto network = get<INetworkLayer>();
		if (!network)
		{
			CPSLOG_ERROR << "Transport and Network layer missing. IDC needs one.";
			return false;
		}

		if (ack)
		{
			CPSLOG_WARN << "Acknowledgment cannot be requested because TransportLayer is missing";
		}

		return network->sendPacket(id, packet);
	}
	return transport->send(id, packet, ack);
}

boost::signals2::connection
IDC::subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle)
{
	auto transport = get<ITransportLayer>();

	if (!transport)
	{
		auto network = get<INetworkLayer>();
		if (!network)
		{
			CPSLOG_ERROR << "Transport and Network layer missing. IDC needs one.";
			return boost::signals2::connection();
		}

		return network->subscribeOnPacket(id, handle);
	}
	return transport->subscribeOnPacket(id, handle);
}

IDCSender
IDC::createSender(const std::string& id)
{
	auto self = get<IDC>();
	if (!self)
	{
		CPSLOG_ERROR << "Something went terribly wrong";
		return IDCSender();
	}

	return IDCSender(self, id);
}
