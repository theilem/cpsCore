/**
 * @file IDCFactory.h
 * @brief Defines the IDCFactory
 * @date Jul 31, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_IDC_NETWORKFACTORY_H_
#define UAVAP_CORE_IDC_NETWORKFACTORY_H_

#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisNetworkLayer.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/INetworkLayer.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialNetworkLayer.h"
#include "cpsCore/Framework/Factory.h"

/**
 * @brief Factory for IInterDeviceComm objects. Default creates SerialIDC
 */
class NetworkFactory: public Factory<INetworkLayer>
{
public:
	NetworkFactory()
	{
		addCreator<SerialNetworkLayer>();
		addCreator<RedisNetworkLayer>();
	}

};

#endif /* UAVAP_CORE_IDC_NETWORKFACTORY_H_ */
