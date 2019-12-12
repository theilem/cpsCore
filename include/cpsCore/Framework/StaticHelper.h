//
// Created by mirco on 26.11.19.
//

#ifndef CPSCORE_STATICHELPER_H
#define CPSCORE_STATICHELPER_H

#include <cpsCore/Aggregation/Aggregator.h>
#include <cpsCore/Configuration/ConfigurableObject.hpp>
#include <iostream>

template<class...>
class StaticHelper;

template<class...Objects>
class StaticHelper
{
public:

	/**
	 * @brief Create an Aggregator containing Objects defined in a configuration loaded in from a config path.
	 * @param configPath path to the configuration .json file
	 * @return Aggregator containing the objects
	 */
	inline Aggregator
	createAggregation(const std::string& configPath)
	{
		return createAggregation(parseConfigFile(configPath));
	}


	/**
	 * @brief Create an Aggregator containing Objects defined in a configuration.
	 * @param config Configuration object tree
	 * @return Aggregator containing the objects
	 */
	inline Aggregator
	createAggregation(const Configuration& config)
	{
		Aggregator agg;
		(addIfInConfig<Objects>(agg, config), ...);
		return agg;
	}

private:

	template<class Object, std::enable_if_t<is_configurable_object<Object>::value, bool> B = true>
	inline void
	addIfInConfig(Aggregator& agg, const Configuration& config)
	{
		auto params = config.get_child_optional(Object::typeId);
		if (params)
		{
			CPSLOG_TRACE << "Adding " << Object::typeId << " with config";
			auto obj = std::make_shared<Object>();
			obj->configure(*params);
			agg.add(obj);
		}
		else
			CPSLOG_TRACE << Object::typeId << " not found in config. It is not added.";
	}

	template<class Object, std::enable_if_t<!is_configurable_object<Object>::value, bool> B = true>
	inline void
	addIfInConfig(Aggregator& agg, const Configuration& config)
	{
		if (config.get_child_optional(Object::typeId))
		{
			CPSLOG_TRACE << "Adding " << Object::typeId << " without config";
			auto obj = std::make_shared<Object>();
			agg.add(obj);
		}
		else
			CPSLOG_TRACE << Object::typeId << " not found in config. It is not added.";
	}


};

#endif //CPSCORE_STATICHELPER_H
