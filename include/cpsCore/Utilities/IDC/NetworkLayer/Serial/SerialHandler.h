/*
 * SerialHandler.h
 *
 *  Created on: Jul 27, 2018
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IDC_NETWORKLAYER_SERIAL_SERIALHANDLER_H_
#define UAVAP_CORE_IDC_NETWORKLAYER_SERIAL_SERIALHANDLER_H_

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialNetworkParams.h"

class SerialHandler
{
public:

	SerialHandler(const SerialNetworkParams& params);

	~SerialHandler();

	using OnPacket = boost::signals2::signal<void(const Packet&)>;

	boost::signals2::connection
	subscribeOnPackets(const OnPacket::slot_type& slot);

	bool
	sendPacket(const Packet& packet);

	void
	startHandler();

	void
	cancelHandler();

private:

	void
	receive(const boost::system::error_code& err, std::size_t bytes_transferred);

	void
	sendStatus(const boost::system::error_code& ec, std::size_t bytes);

	boost::asio::io_service io_;
	boost::asio::serial_port serial_;

	char delim_;
	std::string delimString_;
	bool useCRC_;
	SerialDirection direction_;
	bool sendBlocking_;

	std::stringstream packetBuffer_;


	boost::asio::streambuf inBuffer_;
	boost::asio::streambuf outBuffer_;

	OnPacket onPacket_;
	std::atomic_bool handlerCanceled_;

};

#endif /* UAVAP_CORE_IDC_NETWORKLAYER_SERIAL_SERIALHANDLER_H_ */
