/*
 * RedisChannelParams.h
 *
 *  Created on: Jan 25, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_IDC_NETWORKLAYER_REDIS_REDISCHANNELPARAMS_H_
#define UAVAP_CORE_IDC_NETWORKLAYER_REDIS_REDISCHANNELPARAMS_H_
#include "cpsCore/Configuration/Configuration.hpp"
#include <string>
#include "cpsCore/Configuration/Parameter.hpp"

struct RedisChannelParams
{
	Parameter<std::string> host = {"localhost", "host", false};
	Parameter<std::string> channel = {"", "channel", true};

	template <typename Config>
	void
	configure(Config& config)
	{
		config & host;
		config & channel;
	}
};

struct RedisHostParams
{
	Parameter<std::string> ip = {"127.0.0.1", "ip", false};
	Parameter<std::string> auth = {"", "auth", false};
	Parameter<unsigned int> port = {6379, "port", false};

	template <typename Config>
	void
	configure(Config& config)
	{
		config & ip;
		config & auth;
		config & port;
	}
};

#endif /* UAVAP_CORE_IDC_NETWORKLAYER_REDIS_REDISCHANNELPARAMS_H_ */
