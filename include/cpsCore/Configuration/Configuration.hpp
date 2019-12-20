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

#include <boost/property_tree/ptree.hpp>

using Configuration = boost::property_tree::ptree;
using ConfigurationError = boost::property_tree::ptree_error;

Configuration
parseConfigFile(const std::string& path);

#endif



#endif /* UAVAP_UAVAP_CORE_PROPERTYMAPPER_CONFIGURATION_H_ */
