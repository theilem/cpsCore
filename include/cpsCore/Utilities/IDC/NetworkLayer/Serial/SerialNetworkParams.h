/**
 * @file SerialIDCParams.h
 * @brief Defines SerialIDCParams
 * @date Jul 31, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */
#ifndef UAVAP_CORE_IDC_SERIAL_SERIALIDCPARAMS_H_
#define UAVAP_CORE_IDC_SERIAL_SERIALIDCPARAMS_H_
#include <boost/asio/serial_port_base.hpp>
#include "cpsCore/Utilities/IDC/NetworkLayer/INetworkParams.h"
#include "cpsCore/Utilities/EnumMap.hpp"
#include "cpsCore/Configuration/Configuration.hpp"

enum SerialDirection
{
	BOTH = 0, SEND, RECEIVE
};

ENUMMAP_INIT(SerialDirection, {{SerialDirection::BOTH, "both"}, {SerialDirection::SEND, "send"},
		{SerialDirection::RECEIVE, "receive"}});
/**
 * @brief Parameters used for SerialIDC
 */
struct SerialNetworkParams: public INetworkParams
{
	std::string serialPort; //!< Serial port name e.g. "/dev/ttyUSB0"

	static constexpr unsigned int DEFAULT_BAUD_RATE = 115200;
	unsigned int baudRate = DEFAULT_BAUD_RATE; //!< Baud rate for serial port

	static constexpr auto DEFAULT_DELIMITER_STRING = "*-\n";
	std::string delimiterString = DEFAULT_DELIMITER_STRING; //!< End of transmission delimiter string
	boost::optional<boost::asio::serial_port_base::flow_control::type> flowControl; //!< optional flow control setting

	static constexpr bool DEFAULT_USE_CRC = false;
	bool useCRC = DEFAULT_USE_CRC; //!< Use cyclic redundancy check

	static constexpr SerialDirection DEFAULT_DIRECTION = SerialDirection::BOTH;
	SerialDirection direction = DEFAULT_DIRECTION;

	bool sendBlocking = false;

	SerialNetworkParams() = default;

	SerialNetworkParams(const std::string& port, unsigned int baud_rate, std::string delim);

	bool
	configure(const Configuration& config);

};

#endif /* UAVAP_CORE_IDC_SERIAL_SERIALIDCPARAMS_H_ */
