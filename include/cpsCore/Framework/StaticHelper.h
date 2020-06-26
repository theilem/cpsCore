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

template <typename T>
struct is_static_helper : std::false_type {};

template <class... Objs>
struct is_static_helper<StaticHelper<Objs...>> : std::true_type {};

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
	createAggregation(const Configuration& config)
	{
		return createAggregationImpl<Objects...>(config);
	}

	inline static Aggregator
	createDefaultAggregation(const Configuration& config)
	{
		return createDefaults(config);
	}

private:

	template<class DefaultHelper, class... Objs>
	inline static std::enable_if_t<is_static_helper<DefaultHelper>::value, Aggregator>
	createAggregationImpl(const Configuration& config)
	{
		auto defaultAgg = DefaultHelper::createDefaultAggregation(config);
		auto agg = DefaultHelper::createAggregation(config);

		agg.merge(defaultAgg);
		(addIfInConfig<Objs>(agg, config), ...);
		return agg;
	}

	template<class Obj, class... Others>
	inline static std::enable_if_t<!is_static_helper<Obj>::value, Aggregator>
	createAggregationImpl(const Configuration& config)
	{
		Aggregator agg;
		addIfInConfig<Obj>(agg, config);
		(addIfInConfig<Others>(agg, config), ...);
		return agg;
	}

	inline static Aggregator
	createDefaults(const Configuration& config)
	{
		Aggregator agg;
		(addIfNotInConfig<Objects>(agg, config), ...);
		return agg;
	}

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

	template<class Object>
	inline static void
	addIfNotInConfig(Aggregator& agg, const Configuration& config)
	{
		auto params = config.get_child_optional(Object::typeId);
		if (!params)
		{
			CPSLOG_TRACE << "Adding " << Object::typeId << " without config";
			auto obj = createObjectDefault<Object>();
			agg.add(obj);
		}
		else
			CPSLOG_TRACE << Object::typeId << " found in config. Not creating default.";
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

	template<class Factory, std::enable_if_t<is_static_factory<Factory>::value, bool> B = true>
	inline static typename Factory::ReturnType
	createObjectDefault()
	{
		return Factory::createDefault();
	}

	template<class Object, std::enable_if_t<!is_static_factory<Object>::value, bool> B = true>
	inline static std::shared_ptr<Object>
	createObjectDefault()
	{
		return std::make_shared<Object>();
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
