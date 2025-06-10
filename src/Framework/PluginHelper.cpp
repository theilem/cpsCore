//
// Created by Mirco Theile on 2/6/25.
//

#include "cpsCore/Framework/PluginHelper.h"

#include <dlfcn.h>

PluginHelper&
PluginHelper::instance()
{
    static PluginHelper instance;
    return instance;
}

void
PluginHelper::loadPlugins()
{
    CPSLOG_DEBUG << "PluginHelper: Loading plugins";
    instance().loadPluginsImpl();
    CPSLOG_DEBUG << "PluginHelper: Plugins loaded";
}

std::vector<std::shared_ptr<IAggregatableObject>>
PluginHelper::createPlugins(const Configuration& config)
{
    return instance().createPluginsImpl(config);
}

std::vector<std::shared_ptr<IAggregatableObject>>
PluginHelper::createPluginsImpl(const Configuration& config) const
{
    std::vector<std::shared_ptr<IAggregatableObject>> plugins;
    for (const auto& [key, creator]: pluginCreators_)
    {
        if (auto it = config.find(key); it != config.end())
        {
            CPSLOG_DEBUG << "PluginHelper: Creating plugin" << key;
            plugins.push_back(creator(it.value()));
            CPSLOG_DEBUG << "PluginHelper: Plugin " << key << " created";
        }
    }
    return plugins;
}

void
PluginHelper::loadPluginsImpl()
{
    for (const auto& plugin : params.plugins())
    {
        CPSLOG_DEBUG << "PluginHelper: Loading plugin: " << plugin;
        if (!addPlugin(plugin))
        {
            CPSLOG_ERROR << "PluginHelper: Failed to load plugin: " << plugin;
        }
    }
}

bool
PluginHelper::addPlugin(const std::string& path)
{
    CPSLOG_DEBUG << "PluginHelper: Adding plugin: " << path;
    auto handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle)
    {
        CPSLOG_ERROR << "PluginHelper: Failed to load plugin: " << path << ": " << dlerror();
        return false;
    }
    CPSLOG_DEBUG << "PluginHelper: Plugin loaded successfully";

    auto registerFunc = reinterpret_cast<void (*)()>(dlsym(handle, "register_plugin"));
    if (!registerFunc)
    {
        CPSLOG_ERROR << "PluginHelper: Failed to find register function in plugin: " << path;
        return false;
    }

    registerFunc();
    CPSLOG_DEBUG << "PluginHelper: Plugin registered successfully";

    return true;
}
