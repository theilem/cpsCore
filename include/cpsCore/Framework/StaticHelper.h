/**
 * Created by mirco on 26.11.19.
 * Replace SFINAE with Concepts when c++20 is out
 */

#ifndef CPSCORE_STATICHELPER_H
#define CPSCORE_STATICHELPER_H

#include "cpsCore/Aggregation/Aggregator.h"
#include "cpsCore/Framework/StaticFactory.h"
#include "cpsCore/Framework/PluginHelper.h"

template <class...>
class StaticHelper;

template <typename T>
struct is_static_helper : std::false_type
{
};

template <class... Objs>
struct is_static_helper<StaticHelper<Objs...>> : std::true_type
{
};

template <class... Objects>
class StaticHelper
{
public:
    static_assert((is_complete<Objects>::value && ...),
                  "One of the objects is of incomplete type");

    static_assert(((has_typeid<Objects>::value || is_static_helper<Objects>::value) && ...),
                  "Missing typeid in one of the objects");

    using Types = std::tuple<Objects...>;

    /**
     * @brief Create an Aggregator containing Objects defined in a configuration loaded in from a config path.
     * @param configPath path to the configuration .json file
     * @return Aggregator containing the objects
     */
    static Aggregator
    createAggregation(const std::string& configPath)
    {
        return createAggregation(parseConfigFile(configPath));
    }

    /**
         * @brief Create an Aggregator containing Objects defined in a configuration loaded in from a config path.
         * @param configPath path to the configuration .json file
         * @return Aggregator containing the objects
         */
    static Aggregator
    createAggregation(const std::string& configPath, const std::vector<std::string>& paramOverrides)
    {
        auto config = parseConfigFile(configPath);
        applyParamOverrides(config, paramOverrides);
        return createAggregation(config);
    }


    /**
     * @brief Create an Aggregator containing Objects defined in a configuration.
     * @param config Configuration object tree
     * @return Aggregator containing the objects
     */
    static Aggregator
    createAggregation(const Configuration& config = Configuration())
    {
        auto agg = createAggregationImpl<Objects...>(config);
        if (auto it = config.find(PluginHelper::typeId); it != config.end())
        {
            CPSLOG_DEBUG << "Creating plugins from config";
            if (!PluginHelper::instance().configure(it.value()))
            {
                CPSLOG_ERROR << "PluginHelper::configure failed. Plugins not loaded";
            }
            else
            {
                PluginHelper::loadPlugins();
                auto plugins = PluginHelper::createPlugins(config);
                for (const auto& plugin : plugins)
                {
                    if (plugin)
                    {
                        CPSLOG_DEBUG << "Adding plugin to aggregator";
                        agg.add(plugin);
                    }
                    else
                    {
                        CPSLOG_ERROR << "Plugin is nullptr. Not adding to aggregator.";
                    }
                }
            }
        }
        return agg;
    }
    static Aggregator
    createAggregationNoPlugin(const Configuration& config)
    {
        return createAggregationImpl<Objects...>(config);
    }

    static Aggregator
    createDefaultAggregation(const Configuration& config)
    {
        return createDefaults(config);
    }

private:
    static void
    applyParamOverrides(Configuration& config, const std::vector<std::string>& paramOverrides)
    {
        if (paramOverrides.size() % 2 != 0)
        {
            CPSLOG_ERROR << "Parameter override number not divisible by two...";
            return;
        }
        for (int k = 0; k < paramOverrides.size(); k += 2)
        {
            try
            {
                CPSLOG_DEBUG << "Overriding " << paramOverrides[k] << " from " << config[paramOverrides[k]].get<
                        std::string>()
							 << " to " << paramOverrides[k + 1];
            }
            catch (Configuration::exception&)
            {
            }
            config[paramOverrides[k]] = paramOverrides[k + 1];
        }
    }

    template <class DefaultHelper, class... Objs>
    static std::enable_if_t<is_static_helper<DefaultHelper>::value, Aggregator>
    createAggregationImpl(const Configuration& config)
    {
        auto defaultAgg = DefaultHelper::createDefaultAggregation(config);
        auto agg = DefaultHelper::createAggregationNoPlugin(config);

        agg.merge(defaultAgg);
        (addIfInConfig<Objs>(agg, config), ...);
        return agg;
    }

    template <class Obj, class... Others>
    static std::enable_if_t<!is_static_helper<Obj>::value, Aggregator>
    createAggregationImpl(const Configuration& config)
    {
        Aggregator agg;
        addIfInConfig<Obj>(agg, config);
        (addIfInConfig<Others>(agg, config), ...);
        return agg;
    }

    static Aggregator
    createDefaults(const Configuration& config)
    {
        Aggregator agg;
        (addIfNotInConfig<Objects>(agg, config), ...);
        return agg;
    }

    template <class Object>
    static void
    addIfInConfig(Aggregator& agg, const Configuration& config)
    {
        if (auto params = config.find(Object::typeId); params != config.end())
        {
            CPSLOG_TRACE << "Adding " << Object::typeId << " with config";
            auto obj = createObject<Object>(*params);
            agg.add(obj);
        }
        else
            CPSLOG_TRACE << Object::typeId << " not found in config. It is not added.";
    }

    template <class Object>
    static void
    addIfNotInConfig(Aggregator& agg, const Configuration& config)
    {
        if (auto params = config.find(Object::typeId); params == config.end())
        {
            CPSLOG_TRACE << "Adding " << Object::typeId << " without config";
            auto obj = createObjectDefault<Object>();
            agg.add(obj);
        }
        else
            CPSLOG_TRACE << Object::typeId << " found in config. Not creating default.";
    }

    template <class Factory, std::enable_if_t<is_static_factory<Factory>::value, bool> B = true>
    static typename Factory::ReturnType
    createObject(const Configuration& config)
    {
        return Factory::create(config);
    }

    template <class Object, std::enable_if_t<!is_static_factory<Object>::value, bool> B = true>
    static std::shared_ptr<Object>
    createObject(const Configuration& config)
    {
        auto obj = std::make_shared<Object>();
        configureIfConfigurable(obj, config);
        return obj;
    }

    template <class Factory, std::enable_if_t<is_static_factory<Factory>::value, bool> B = true>
    static typename Factory::ReturnType
    createObjectDefault()
    {
        return Factory::createDefault();
    }

    template <class Object, std::enable_if_t<!is_static_factory<Object>::value, bool> B = true>
    static std::shared_ptr<Object>
    createObjectDefault()
    {
        return std::make_shared<Object>();
    }


    template <class Object, std::enable_if_t<is_configurable_object<Object>::value, bool> B = true>
    static void
    configureIfConfigurable(std::shared_ptr<Object> obj, const Configuration& config)
    {
        obj->configure(config);
    }


    template <class Object, std::enable_if_t<!is_configurable_object<Object>::value, bool> B = true>
    static void
    configureIfConfigurable(std::shared_ptr<Object>, const Configuration&)
    {
    }
};

#endif //CPSCORE_STATICHELPER_H
