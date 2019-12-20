/**
 * @file IDCFactory.h
 * @brief Defines the IDCFactory
 * @date 14. Dec 2019
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_IDC_NETWORKFACTORY_H_
#define UAVAP_CORE_IDC_NETWORKFACTORY_H_

#include "cpsCore/Framework/StaticFactory.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/INetworkLayer.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisNetworkLayer.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialNetworkLayer.h"

/**
 * @brief Factory for IInterDeviceComm objects.
 */
using NetworkFactory = StaticFactory<INetworkLayer, true, SerialNetworkLayer, RedisNetworkLayer>;

#endif /* UAVAP_CORE_IDC_NETWORKFACTORY_H_ */
