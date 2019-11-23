/**
 * @file IInterDeviceComm.h
 * @brief Defines the Interface class IInterDeviceComm
 * @date Jul 31, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_IDC_INETWORKLAYER_H_
#define UAVAP_CORE_IDC_INETWORKLAYER_H_
#include "cpsCore/Utilities/Packet.h"

#include <boost/signals2.hpp>

struct INetworkParams;

/**
 * @brief Interface class for the inter device communication (IDC).
 *
 * For IDC an InterDeviceComm should have the ability to create a sender to send packets and subscribe on
 * incoming packets by calling a handle.
 */
class INetworkLayer
{
public:

	static constexpr const char* const typeId = "network";

	virtual
	~INetworkLayer() = default;

	/**
	 * @brief 	Create a Sender object that allows to send packets though IDC using the IIDCParams
	 * 			defined in params
	 * @param params Parameters defining the IDC settings
	 * @return A Sender object
	 */
	virtual bool
	sendPacket(const std::string& id, const Packet& packet) = 0;

	using OnPacket = boost::signals2::signal<void(const Packet&)>;

	/**
	 * @brief Subscribe on packets coming through IDC. Uses the params to set up the reception.
	 * @param params Parameters defining the IDC settings
	 * @param handle Handle to be called with the received packet
	 */
	virtual boost::signals2::connection
	subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle) = 0;
};

#endif /* UAVAP_CORE_IDC_INETWORKLAYER_H_ */
