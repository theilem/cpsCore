//
// Created by mirco on 14.12.19.
//

#ifndef CPSCORE_STATICFACTORY_H
#define CPSCORE_STATICFACTORY_H

#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Configuration/Configuration.hpp"
#include "cpsCore/Configuration/TypeTraits.hpp"

template<class SuperClass, bool SUPPORT_MULTI, class...Objects>
class StaticFactory
{
public:

	static constexpr const char* const typeId = SuperClass::typeId;
	static constexpr bool supportsMulti = SUPPORT_MULTI;
	using ReturnTypeSingle = std::shared_ptr<SuperClass>;
	using ReturnTypeMulti = std::vector<std::shared_ptr<SuperClass>>;
	using ReturnType = std::conditional_t<supportsMulti, ReturnTypeMulti, ReturnTypeSingle>;
	using SpecificObjects = std::tuple<Objects...>;

	static ReturnType
	create(const Configuration& config)
	{
		if (config.empty())
		{
			CPSLOG_ERROR << "Config is empty. Cannot create objects of factory";
			return ReturnType();
		}
		if (!supportsMulti && (config.size() > 1))
		{
			CPSLOG_ERROR << "Factory " << typeId << " does not support multi (" << config.size()
						 << " were found). Will create first only.";
		}

		if constexpr(supportsMulti)
		{
			ReturnType objects;
			for (const auto& it : config)
			{
				auto specific = createSpecific<Objects...>(it.first, it.second);
				if (specific)
					objects.push_back(specific);
			}
			return objects;
		}
		else
		{
			auto child = config.front();
			return createSpecific<Objects...>(child.first, child.second);
		}
	}

	static ReturnType
	create(const std::string& type)
	{
		return createSpecific<Objects...>(type);
	}

	static ReturnTypeSingle
	createDefault()
	{
		return createDefaultImpl<Objects...>();
	}

private:

	template<class FirstObj, class...>
	static ReturnTypeSingle
	createDefaultImpl()
	{
		return std::make_shared<FirstObj>();
	}

	template<class Object>
	static ReturnTypeSingle
	createSpecific(const std::string& type)
	{
		if (type.compare(Object::typeId) == 0)
		{
			return std::make_shared<Object>();
		}
		CPSLOG_ERROR << "Type " << type << " of " << typeId << " not found";
		return nullptr;
	}

	template<class Object, class Second, class...Others>
	static ReturnTypeSingle
	createSpecific(const std::string& type)
	{
		if (type.compare(Object::typeId) == 0)
		{
			return std::make_shared<Object>();
		}
		return createSpecific<Second, Others...>(type);
	}

	template<class Object>
	static ReturnTypeSingle
	createSpecific(const std::string& type, const Configuration& config)
	{
		if (type.compare(Object::typeId) == 0)
		{
			auto object = std::make_shared<Object>();
			if constexpr (is_configurable_object<Object>::value)
				object->configure(config);
			return object;
		}
		CPSLOG_ERROR << "Type " << type << " of " << typeId << " not found";
		return nullptr;
	}

	template<class Object, class Second, class...Others>
	static ReturnTypeSingle
	createSpecific(const std::string& type, const Configuration& config)
	{
		if (type.compare(Object::typeId) == 0)
		{
			auto object = std::make_shared<Object>();
			if constexpr (is_configurable_object<Object>::value)
				object->configure(config);
			return object;
		}
		return createSpecific<Second, Others...>(type, config);
	}

};

template<class T>
struct is_static_factory : std::false_type
{
};

template<class T, bool S, class...C>
struct is_static_factory<StaticFactory<T, S, C...>> : std::true_type
{
};


#endif //CPSCORE_STATICFACTORY_H
