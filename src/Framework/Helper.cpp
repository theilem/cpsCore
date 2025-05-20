/*
 * Helper.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: mircot
 */
#include "cpsCore/Framework/PluginManager.h"
#include "cpsCore/Framework/Helper.h"
#include "cpsCore/Aggregation/Aggregator.h"

Aggregator
Helper::createAggregation(const std::string& configPath)
{
    auto conf = parseConfigFile(configPath);
    if (conf.empty())
    {
        CPSLOG_ERROR << "Configuration file not found or empty.";
        return {};
    }
    return createAggregation(conf);
}

Aggregator
Helper::createAggregation(const Configuration& conf)
{
    Aggregator agg;

    // PluginManager plugin;
    // auto child = conf.get_child_optional("plugin_manager");
    // if (child)
    // {
    // 	plugin.configure(*child);
    // }

    for (auto& [key, value] : conf.items())
    {
        // Ignore plugin manager
        // if (it.first.compare("plugin_manager") == 0)
        // 	continue;

        //Remove from default factories
        defaultCreators_.erase(key);

        auto creator = creators_.find(key);
        if (creator == creators_.end())
        {
            CPSLOG_ERROR << "Factory for " << key << " not found. Ignore.";
            continue;
        }

        try
        {
            CPSLOG_TRACE << "Creating and configuring " << key;
            auto obj = creator->second(value);
            agg.add(obj);
            CPSLOG_TRACE << "Added " << key << " to aggregation";
        }
        catch (FrameworkError& err)
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
        }
        catch (FrameworkError& err)
        {
            CPSLOG_ERROR << "Exception: " << err.what();
        }
    }

    return agg;
}

// void
// Helper::mergeGlobalConfig(Configuration& config,
// 		const Configuration& globalConf)
// {
// 	for (auto& confIt : config)
// 	{
// 		for (auto& it : globalConf)
// 		{
// 			confIt.second.push_back(it);
// 		}
// 	}
// }
