//
// Created by mirco on 20.11.20.
//

#ifndef CPSCORE_GLOBALCONFIGURATION_HPP
#define CPSCORE_GLOBALCONFIGURATION_HPP

#include <cpsCore/Configuration/ConfigurableObject.hpp>
#include <cpsCore/Aggregation/AggregatableObject.hpp>

template<class Params>
class GlobalConfiguration : public ConfigurableObject<Params>, public AggregatableObject<>
{
public:

	static constexpr TypeId typeId = "global_config";

	const Params&
	getParams() const
	{
		return this->params;
	}
};


#endif //CPSCORE_GLOBALCONFIGURATION_HPP
