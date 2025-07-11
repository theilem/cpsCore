/*
 * Configuration.h
 *
 *  Created on: Jun 12, 2019
 *      Author: mirco
 */

#ifndef UAVAP_UAVAP_CORE_PROPERTYMAPPER_CONFIGURATION_H_
#define UAVAP_UAVAP_CORE_PROPERTYMAPPER_CONFIGURATION_H_

#ifdef NO_RTTI

#include "cpsCore/Configuration/EmptyConfiguration.h"

using Configuration = EmptyConfiguration;
using ConfigurationError = int;

#else

#include <nlohmann/json.hpp>

using Configuration = nlohmann::json;

class IConfigurableObject
{
public:
	virtual ~IConfigurableObject() = default;
	virtual bool
	configure(const Configuration& config) = 0;
	virtual void
	parse(Configuration& config) = 0;
};

Configuration
parseConfigFile(const std::string& path);

Configuration
parseConfigString(const std::string& config);

#endif



#endif /* UAVAP_UAVAP_CORE_PROPERTYMAPPER_CONFIGURATION_H_ */
