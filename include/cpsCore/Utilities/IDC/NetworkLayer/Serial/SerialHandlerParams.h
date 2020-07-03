//
// Created by mirco on 19.06.20.
//

#ifndef CPSCORE_SERIALHANDLERPARAMS_H
#define CPSCORE_SERIALHANDLERPARAMS_H

#include "cpsCore/Utilities/EnumMap.hpp"
#include "cpsCore/Configuration/Parameter.hpp"

#include <boost/asio/serial_port_base.hpp>

enum SerialDirection
{
	BOTH = 0, SEND, RECEIVE
};

ENUMMAP_INIT(SerialDirection, {
	{ SerialDirection::BOTH, "both" },
	{ SerialDirection::SEND, "send" },
	{ SerialDirection::RECEIVE, "receive" }
});

using FlowControl = boost::asio::serial_port_base::flow_control::type;

ENUMMAP_INIT(FlowControl, {
	{ boost::asio::serial_port_base::flow_control::software, "sw" },
	{ boost::asio::serial_port_base::flow_control::hardware, "hw" },
	{ boost::asio::serial_port_base::flow_control::none, "none" }
});

struct SerialHandlerParams
{
	Parameter<std::string> serialPort = {"/dev/ttyUSB0", "serial_port", true};
	Parameter<unsigned> baudRate = {115200, "baud_rate", true};
	Parameter<std::string> delimiterString = {"*-*", "delimiter_string", true};
	Parameter<bool> useCRC = {true, "use_crc", true};
	Parameter<FlowControl> flowControl = {boost::asio::serial_port_base::flow_control::none, "flow_control", false};
	Parameter<SerialDirection> direction = {SerialDirection::BOTH, "direction", false};
	Parameter<bool> sendBlocking = {false, "send_blocking", false};

	template <typename Config>
	inline void
	configure(Config& c)
	{
		c & serialPort;
		c & baudRate;
		c & delimiterString;
		c & useCRC;
		c & flowControl;
		c & direction;
		c & sendBlocking;
	}
};


#endif //CPSCORE_SERIALHANDLERPARAMS_H
