/**
 * @file SerialIDC.h
 * @brief Defines SerialIDC
 * @date Jun 12, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */
#ifndef GROUNDCOMMUNICATION_SERIALCONNECTION_H_
#define GROUNDCOMMUNICATION_SERIALCONNECTION_H_

#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/signals2/signal.hpp>

#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Configuration/Configuration.hpp"
#include "cpsCore/Synchronization/IRunnableObject.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialHandler.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/INetworkLayer.h"

class IScheduler;
class SignalHandler;

/**
 * @brief IDC using serial to communicate among devices
 */
class SerialNetworkLayer: public INetworkLayer,
		public AggregatableObject<IScheduler, SignalHandler>,
		public IRunnableObject
{

public:

	static constexpr TypeId typeId = "serial";

	~SerialNetworkLayer();

	/**
	 * @brief Create the SerialIDC using a config tree
	 * @param config Configuration tree
	 * @return IInterDeviceComm ptr of a SerialIDC
	 */
	static std::shared_ptr<INetworkLayer>
	create(const Configuration& config);

	bool
	configure(const Configuration& config);

	bool
	sendPacket(const std::string& id, const Packet& packet) override;

	/**
	 * @brief Subscribe on incoming packets. Uses params to create serial port. Needs scheduler to
	 * 		dedicate a thread for waiting for reception.
	 * @param params SerialIDCParams defining the serial port etc.
	 * @param handle handler to be called with incoming packet
	 */
	boost::signals2::connection
	subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle) override;

	bool
	run(RunStage stage) override;

private:

	void
	onSigInt(int sig);

	std::unordered_map<std::string, std::shared_ptr<SerialHandler> > handler_;

};

#endif /* GROUNDCOMMUNICATION_SERIALCONNECTION_H_ */
