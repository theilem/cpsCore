/**
 * @file Factory.h
 * @brief Definition of the Factory class used for the different Factories in cpsCore
 * @date Jul 26, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_FRAMEWORK_FACTORY_H_
#define UAVAP_CORE_FRAMEWORK_FACTORY_H_

#include "cpsCore/Configuration/Configuration.hpp"
#include "cpsCore/Framework/PluginRestriction.h"
#include "cpsCore/Framework/FrameworkExceptions.h"
#include "cpsCore/Utilities/EnumMap.hpp"

/**
 * @brief Template class for of a typical factory.
 *
 * The template type defines the Interface class which has to be implemented by all the classes that can
 * be created by the factory. The factory searches for the "type" argument in the configuration tree
 * to determine the class to create. In the constructor of any factory implementing this factory the
 * creatorMap_ has to be filled by registering creators using the addCreator function. If a Factory should
 * have a default class to create, the setDefault method should reference the string used for that specific
 * creator.
 */
template<class Type>
class Factory
{

public:

	virtual
	~Factory() = default;

	/**
	 * @brief 	Create function called by a helper. Uses "type" argument in config to determine the class to
	 * 			be created
	 * @param config Configuration tree
	 * @param plugin
	 * @return Created object
	 */
	std::shared_ptr<Type>
	create(const Configuration& config);

	void
	setPluginRestriction(PluginRestriction plug);

	using TypeId = const char* const;

	static TypeId
	typeId();

	template<class SpecificType>
	static void
	registerExternalCreator();

	std::vector<std::string>
	getTypeIds() const;

protected:

	//!Defines the function that can be registered
	using Creator = std::function<std::shared_ptr<Type>(const Configuration&)>;

	/**
	 * @brief Adds a creator function with a string id.
	 * @param id ID of that objects creator
	 * @param creator The objects creator that creates the object using a config tree
	 */
	template<class SpecificType>
	void
	addCreator();

	/**
	 * @brief Set a default id. If no type is set in the tree this default object is created.
	 * @param defaultId Default id.
	 */
	template<class SpecificType>
	void
	setDefault();

	template<class SpecificType>
	void
	addConfigurable();

private:

	template<class Configurable>
	static std::shared_ptr<Configurable>
	createConfigurable(const Configuration& config);

	std::map<std::string, Creator> creatorMap_; //!< Map containing all the creators and their ids

	static std::map<std::string, Creator> pluginMap_;
	PluginRestriction pluginRestriction_ = PluginRestriction::NOT_ALLOWED;

	std::string defaultId_; //!< The default id

};

template<class Type>
std::map<std::string, typename Factory<Type>::Creator> Factory<Type>::pluginMap_;

template<class Type>
inline std::shared_ptr<Type>
Factory<Type>::create(const Configuration& config)
{
	if (creatorMap_.empty() && pluginRestriction_ != PluginRestriction::ALLOWED)
	{
		throw FactoryInitializationError("Creator Map is empty.");
	}

	std::string type;

	try
	{
		type = config["type"];
	} catch (Configuration::exception&)
	{
		if (defaultId_.empty())
			throw InvalidTypeError("Type missing in config.");

		type = defaultId_;
	}

	auto it = creatorMap_.find(type);

	Creator creator;

	if (it == creatorMap_.end())
	{
		if (pluginRestriction_ == PluginRestriction::ALLOWED)
		{
			auto plugIt = pluginMap_.find(type);
			if (plugIt != creatorMap_.end())
			{
				if (plugIt->second)
					return plugIt->second(config);
				else
				{
					CPSLOG_ERROR << "Create function in plugin " << type << " not set";
					throw FactoryInitializationError("Plugin invalid");
				}
			}

		}
		throw InvalidTypeError(
				"Unknown Type " + type + " in Factory. Plugins are "
				+ EnumMap<PluginRestriction>::convert(pluginRestriction_));
	}
	else
		creator = it->second;

	return creator(config);
}

template<class Type>
template<class SpecificType>
inline void
Factory<Type>::addCreator()
{
	std::string type = SpecificType::typeId;
	if (creatorMap_.find(type) != creatorMap_.end())
	{
		CPSLOG_ERROR << "Same id for different creator added. Ignore.";
		return;
	}

	creatorMap_.insert(std::make_pair(type, &SpecificType::create));
}

template<class Type>
template<class SpecificType>
inline void
Factory<Type>::setDefault()
{
	defaultId_ = SpecificType::typeId;
}

template<class Type>
template<class SpecificType>
inline void
Factory<Type>::registerExternalCreator()
{
	std::string type = SpecificType::typeId;
	if (pluginMap_.find(type) != pluginMap_.end())
	{
		CPSLOG_ERROR << "Same id for different plugin added. Ignore.";
		return;
	}
	CPSLOG_DEBUG << "Registering creator '" << type << "' for '" << Type::typeId << "'";

	pluginMap_.insert(std::make_pair(type, &SpecificType::create));
}

template<class Type>
inline void
Factory<Type>::setPluginRestriction(PluginRestriction plug)
{
	pluginRestriction_ = plug;
}

template<class Type>
inline const char* const
Factory<Type>::typeId()
{
	return Type::typeId;
}

template<class Type>
inline std::vector<std::string>
Factory<Type>::getTypeIds() const
{
	std::vector<std::string> typeIds;

	for (const auto& it : creatorMap_)
		typeIds.push_back(it.first);
	for (const auto& it : pluginMap_)
		typeIds.push_back(it.first);

	return typeIds;
}

template<class Type>
template<class SpecificType>
inline void
Factory<Type>::addConfigurable()
{
	std::string type = SpecificType::typeId;
	if (creatorMap_.find(type) != creatorMap_.end())
	{
		CPSLOG_ERROR << "Same id for different creator added. Ignore.";
		return;
	}

	creatorMap_.insert(std::make_pair(type, &Factory<Type>::createConfigurable<SpecificType>));
}

template<class Type>
template<class Configurable>
inline std::shared_ptr<Configurable>
Factory<Type>::createConfigurable(const Configuration& config)
{
	auto obj = std::make_shared<Configurable>();
	if (!obj->configure(config))
	{
		CPSLOG_ERROR << Configurable::typeId << ": configuration failed.";
	}
	return obj;
}

#endif /* UAVAP_CORE_FRAMEWORK_FACTORY_H_ */
