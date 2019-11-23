/**
 * @file PluginRestriction.h
 * @date Sep 20, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#ifndef UAVAP_CORE_FRAMEWORK_PLUGINRESTRICTION_H_
#define UAVAP_CORE_FRAMEWORK_PLUGINRESTRICTION_H_

#include "cpsCore/Utilities/EnumMap.hpp"

enum class PluginRestriction
{
	DEFAULT, ALLOWED, NOT_ALLOWED
};

ENUMMAP_INIT(PluginRestriction,
			 {
				 { PluginRestriction::DEFAULT, "default" },
				 { PluginRestriction::ALLOWED, "allowed" },
				 { PluginRestriction::NOT_ALLOWED, "not_allowed" }
			 });

#endif /* UAVAP_CORE_FRAMEWORK_PLUGINRESTRICTION_H_ */
