/*
 * SerialIDCParams.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: mircot
 */
#include <boost/property_tree/ptree.hpp>
#include "cpsCore/Configuration/PropertyMapper.hpp"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialNetworkParams.h"


SerialNetworkParams::SerialNetworkParams(const std::string& port, unsigned int baud_rate, std::string delim) :
		serialPort(port), baudRate(baud_rate), delimiterString(delim)
{
}

bool
SerialNetworkParams::configure(const Configuration& config)
{
	PropertyMapper<Configuration> pm(config);
	pm.add("serial_port", serialPort, true);
	pm.add<unsigned int>("baud_rate", baudRate, false);
	pm.add("delimiter_string", delimiterString, false);
	pm.add<bool>("use_crc", useCRC, false);
	pm.add<bool>("send_blocking", sendBlocking, false);

	std::string flowCTRL;
	if (pm.add("flow_controle", flowCTRL, false))
	{
		if (flowCTRL.compare("none") == 0)
		{
			flowControl = boost::asio::serial_port_base::flow_control::none;
		}
		else if (flowCTRL.compare("hw") == 0)
		{
			flowControl = boost::asio::serial_port_base::flow_control::hardware;
		}
		else if (flowCTRL.compare("sw") == 0)
		{
			flowControl = boost::asio::serial_port_base::flow_control::software;
		}
		else
		{
			CPSLOG_ERROR << "Flowcontrol type unknown, available: [none, hw, sw]";
		}
	}
	std::string dir;
	if (pm.add("direction", dir, false))
	{
		direction = EnumMap<SerialDirection>::convert(dir);
	}
	return pm.map();
}
