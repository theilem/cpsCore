//
// Created by mirco on 19.06.20.
//

#ifndef CPSCORE_SERIALNETWORKLAYERPARAMS_H
#define CPSCORE_SERIALNETWORKLAYERPARAMS_H

#include <vector>

#include "cpsCore/Configuration/Parameter.hpp"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialHandlerParams.h"

struct SerialNetworkLayerParams
{
	Parameter<std::vector<SerialHandlerParams>> ports = {{SerialHandlerParams()}, "ports",  true};

	template <typename Config>
	inline void
	configure(Config& c)
	{
		c & ports;
	}
};


#endif //CPSCORE_SERIALNETWORKLAYERPARAMS_H
