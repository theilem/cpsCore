/*
 * Helper.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: mircot
 */
#include <boost/property_tree/json_parser.hpp>
#include "cpsCore/Framework/PluginManager.h"
#include "cpsCore/Framework/Helper.h"
#include "cpsCore/Aggregation/Aggregator.h"

Aggregator
Helper::createAggregation(const std::string& configPath)
{
	Configuration conf;
	if (!configPath.empty())
	{
		boost::property_tree::read_json(configPath, conf);
	}

	try
	{
		auto globalConf = conf.get_child("global");
		mergeGlobalConfig(conf, globalConf);
	} catch (ConfigurationError&)
	{
		//No global config
	}

	return createAggregation(conf);
}

Aggregator
Helper::createAggregation(const Configuration& conf)
{
	Aggregator agg;

	PluginManager plugin;
	auto child = conf.get_child_optional("plugin_manager");
	if (child)
	{
		plugin.configure(*child);
	}

	for (auto& it : conf)
	{
		// Ignore plugin manager
		if (it.first.compare("plugin_manager") == 0)
			continue;

		//Remove from default factories
		defaultCreators_.erase(it.first);

		auto creator = creators_.find(it.first);
		if (creator == creators_.end())
		{
			CPSLOG_ERROR << "Factory for " << it.first << " not found. Ignore.";
			continue;
		}

		try
		{
			CPSLOG_TRACE << "Creating and configuring " << it.first;
			auto obj = creator->second(it.second);
			agg.add(obj);
			CPSLOG_TRACE << "Added " << it.first << " to aggregation";
		} catch (FrameworkError& err)
		{
			CPSLOG_ERROR << "Exception: " << err.what();
		}
	}

	for (auto& it : defaultCreators_)
	{
		try
		{
			auto obj = it.second();
			agg.add(obj);
		} catch (FrameworkError& err)
		{
			CPSLOG_ERROR << "Exception: " << err.what();
		}
	}

	return agg;
}

void
Helper::mergeGlobalConfig(Configuration& config,
		const Configuration& globalConf)
{
	for (auto& confIt : config)
	{
		for (auto& it : globalConf)
		{
			confIt.second.push_back(it);
		}
	}
}
