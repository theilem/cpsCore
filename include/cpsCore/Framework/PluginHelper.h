//
// Created by Mirco Theile on 2/6/25.
//

#ifndef PLUGINHELPER_H
#define PLUGINHELPER_H
#include "cpsCore/Aggregation/IAggregatableObject.h"
#include "cpsCore/Configuration/ConfigurableObject.hpp"

extern "C"
{
    void
    register_plugin();
}

class IAggregatableObject;

struct PluginHelperParams
{
    Parameter<std::vector<std::string>> plugins = {{}, "plugins", true};

    template <typename Configurator>
    void
    configure(Configurator& c)
    {
        c & plugins;
    }
};

class PluginHelper : public ConfigurableObject<PluginHelperParams>
{
public:
    static constexpr auto typeId = "plugin_helper";
    using CreatorFunc = std::function<std::shared_ptr<IAggregatableObject>(const Configuration&)>;
    static PluginHelper&
    instance();

    static void
    loadPlugins();

    static std::vector<std::shared_ptr<IAggregatableObject>>
    createPlugins(const Configuration& config);

    template <typename Type>
    static void
    registerPlugin()
    {
        instance().registerPluginImpl<Type>();
    }

private:
    PluginHelper() = default;
    std::vector<std::shared_ptr<IAggregatableObject>>
    createPluginsImpl(const Configuration& config) const;

    void
    loadPluginsImpl();

    bool
    addPlugin(const std::string& path);

    template <typename Type>
    void
    registerPluginImpl()
    {
        auto type = Type::typeId;
        if (pluginCreators_.find(type) != pluginCreators_.end())
        {
            CPSLOG_ERROR << "Plugin with id '" << type << "' already registered. Ignoring.";
            return;
        }
        CPSLOG_DEBUG << "Registering plugin creator for '" << type << "'";
        if constexpr (is_configurable_object<Type>::value)
        {
            pluginCreators_.insert(std::make_pair(type, [type](const Configuration& config) -> std::shared_ptr<IAggregatableObject>
            {
                auto plugin = std::make_shared<Type>();
                if (!plugin->configure(config))
                {
                    CPSLOG_ERROR << "Failed to configure plugin of type '" << type << "'";
                }
                return plugin;
            }));
        }
        else
        {
            pluginCreators_.insert(std::make_pair(type, [](const Configuration& config) -> std::shared_ptr<IAggregatableObject>
            {
                return std::make_shared<Type>();
            }));
        }
    }

    std::unordered_map<std::string, CreatorFunc> pluginCreators_;
};

#endif //PLUGINHELPER_H
