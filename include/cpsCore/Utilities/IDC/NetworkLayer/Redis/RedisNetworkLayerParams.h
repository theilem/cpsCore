//
// Created by mirco on 16.12.19.
//

#ifndef CPSCORE_REDISNETWORKLAYERPARAMS_H
#define CPSCORE_REDISNETWORKLAYERPARAMS_H


#include "cpsCore/Configuration/Parameter.hpp"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisChannelParams.h"


struct RedisNetworkLayerParams
{
	Parameter<std::vector<RedisChannelParams>> sub = {{RedisChannelParams()}, "sub", false};
	Parameter<std::vector<RedisChannelParams>> pub = {{RedisChannelParams()}, "pub", false};

	template <typename Config>
	void
	configure(Config& config)
	{
		config & sub;
		config & pub;
	}
};


#endif //CPSCORE_REDISNETWORKLAYERPARAMS_H
