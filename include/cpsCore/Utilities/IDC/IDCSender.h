/*
 * Sender.h
 *
 *  Created on: Jul 28, 2018
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IDC_IDCSENDER_H_
#define UAVAP_CORE_IDC_IDCSENDER_H_
#include <memory>
#include <string>

class IDC;
class Packet;

class IDCSender
{
public:

	IDCSender() = default;

	IDCSender(std::weak_ptr<IDC> idc, const std::string& id);

	bool
	sendPacket(const Packet& packet, bool ack = false);

private:

	std::weak_ptr<IDC> idc_;
	std::string id_;
};


#endif /* UAVAP_CORE_IDC_IDCSENDER_H_ */
