//
// Created by mirco on 26.11.19.
//

#ifndef CPSCORE_STATICHELPER_H
#define CPSCORE_STATICHELPER_H

#include <cpsCore/Aggregation/Aggregator.h>
#include <cpsCore/Configuration/ConfigurableObject.hpp>

template<class...>
class StaticHelper;

template<class...Objects>
class StaticHelper
{
public:

	using Types = std::tuple<Objects...>;

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

	template<class Object, typename std::enable_if<is_configurable_object<Object>::value, bool>::type B = true>
	inline void
	addIfInConfig(Aggregator& agg, const Configuration& config)
	{
		auto params = config.get_child_optional(Object::typeId);
		if (params)
		{
			auto obj = std::make_shared<Object>();
			obj->configure(*params);
			agg.add(obj);
		}
	}

	template<class Object, typename std::enable_if<!is_configurable_object<Object>::value, bool>::type B = true>
	inline void
	addIfInConfig(Aggregator& agg, const Configuration& config)
	{
		if (config.get_child_optional(Object::typeId))
		{
			auto obj = std::make_shared<Object>();
			agg.add(obj);
		}
	}


};

#endif //CPSCORE_STATICHELPER_H
