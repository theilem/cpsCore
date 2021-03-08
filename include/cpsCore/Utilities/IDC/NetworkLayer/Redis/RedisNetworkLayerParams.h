//
// Created by mirco on 16.12.19.
//

#ifndef CPSCORE_REDISNETWORKLAYERPARAMS_H
#define CPSCORE_REDISNETWORKLAYERPARAMS_H


#include "cpsCore/Configuration/Parameter.hpp"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisChannelParams.h"


struct RedisNetworkLayerParams
{
	Parameter<std::map<std::string, RedisHostParams>> hosts = {{{"localhost", RedisHostParams()}}, "hosts",
																false};
	Parameter<std::map<std::string, RedisChannelParams>> sub = {{{"default", RedisChannelParams()}}, "sub",
																false};
	Parameter<std::map<std::string, RedisChannelParams>> pub = {{{"default", RedisChannelParams()}}, "pub",
																false};

	template<typename Config>
	void
	configure(Config& config)
	{
		config & hosts;
		config & sub;
		config & pub;
	}
};


#endif //CPSCORE_REDISNETWORKLAYERPARAMS_H
