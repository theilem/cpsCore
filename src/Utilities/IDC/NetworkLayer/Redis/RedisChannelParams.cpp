/*
 * RedisChannelParams.cpp
 *
 *  Created on: Jan 25, 2019
 *      Author: mirco
 */
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisChannelParams.h"
#include "cpsCore/Configuration/PropertyMapper.hpp"

bool
RedisChannelParams::configure(const Configuration& config)
{
	PropertyMapper<Configuration> pm(config);
	pm.add("host_ip", hostIP_, false);
	pm.add("auth", auth_, false);
	pm.add<unsigned int>("port", port_, false);
	pm.add("channel", channel_, true);

	return pm.map();
}
