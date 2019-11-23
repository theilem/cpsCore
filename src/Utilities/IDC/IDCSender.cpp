/*
 * IDCSender.cpp
 *
 *  Created on: Jul 28, 2018
 *      Author: mircot
 */

#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/IDC/IDC.h"
#include "cpsCore/Utilities/IDC/IDCSender.h"
#include "cpsCore/Logging/CPSLogger.h"


IDCSender::IDCSender(std::weak_ptr<IDC> idc, const std::string& id) :
		idc_(idc), id_(id)
{
}

bool
IDCSender::sendPacket(const Packet& packet, bool ack)
{
	auto idc = idc_.lock();
	if (!idc)
	{
		CPSLOG_WARN << "IDC not available";
		return false;
	}

	return idc->sendPacket(id_, packet);
}
