/**
 * Created by mirco on 26.11.19.
 * Replace SFINAE with Concepts when c++20 is out
 */

#ifndef CPSCORE_STATICHELPER_H
#define CPSCORE_STATICHELPER_H

#include "cpsCore/Aggregation/Aggregator.h"
#include "cpsCore/Framework/StaticFactory.h"
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
	inline static Aggregator
	createAggregation(const std::string& configPath)
	{
		return createAggregation(parseConfigFile(configPath));
	}


	/**
	 * @brief Create an Aggregator containing Objects defined in a configuration.
	 * @param config Configuration object tree
	 * @return Aggregator containing the objects
	 */
	inline static Aggregator
	createAggregation(const Configuration& config = Configuration())
	{
		Aggregator agg;
		(addIfInConfig<Objects>(agg, config), ...);
		return agg;
	}

private:

	template<class Object>
	inline static void
	addIfInConfig(Aggregator& agg, const Configuration& config)
	{
		auto params = config.get_child_optional(Object::typeId);
		if (params)
		{
			CPSLOG_TRACE << "Adding " << Object::typeId << " with config";
			auto obj = createObject<Object>(*params);
			agg.add(obj);
		}
		else
			CPSLOG_TRACE << Object::typeId << " not found in config. It is not added.";
	}

	template<class Factory, std::enable_if_t<is_static_factory<Factory>::value, bool> B = true>
	inline static typename Factory::ReturnType
	createObject(const Configuration& config)
	{
		return Factory::create(config);
	}

	template<class Object, std::enable_if_t<!is_static_factory<Object>::value, bool> B = true>
	inline static std::shared_ptr<Object>
	createObject(const Configuration& config)
	{
		auto obj = std::make_shared<Object>();
		configureIfConfigurable(obj, config);
		return obj;
	}


	template<class Object, std::enable_if_t<is_configurable_object<Object>::value, bool> B = true>
	static void
	configureIfConfigurable(std::shared_ptr<Object> obj, const Configuration& config)
	{
		obj->configure(config);
	}


	template<class Object, std::enable_if_t<!is_configurable_object<Object>::value, bool> B = true>
	static void
	configureIfConfigurable(std::shared_ptr<Object>, const Configuration&)
	{
	}


};

#endif //CPSCORE_STATICHELPER_H
