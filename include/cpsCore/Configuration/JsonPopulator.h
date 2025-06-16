/*
 * JsonPopulator.h
 *
 *  Created on: Jul 23, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_
#define UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_

#include <fstream>
#include <type_traits>

#include "cpsCore/Configuration/ConfigurableObject.hpp"
#include "cpsCore/Framework/StaticFactory.h"
#include "cpsCore/Framework/StaticHelper.h"
#include "uavAP/FlightControl/Controller/PIDController/PIDHandling.h"


class JsonPopulator
{
public:
    JsonPopulator() = default;

    ~JsonPopulator() = default;

    template <typename Type>
    JsonPopulator&
    operator&(const Parameter<Type>& param)
    {
        if constexpr (is_parameter_set<Type>::value || is_parameter_set_ref<Type>::value)
        {
            JsonPopulator pop;
            const_cast<Type&>(param.value).configure(pop);
            json_[param.id] = pop.json_;
        }
        else
        {
            writeValue(param.id, param.value);
        }
        return *this;
    }

    template <typename Type>
    JsonPopulator&
    operator&(const ParameterRef<Type>& param)
    {
        if constexpr (is_configurable_object<Type>::value)
        {
            JsonPopulator pop;
            const_cast<Type&>(param.value).parse(pop);
            if constexpr (has_configure_params<Type>::value)
            {
                const_cast<Type&>(param.value).configureParams(pop);
            }
            json_[param.id] = pop.json_;
        }
        else
        {
            writeValue(param.id, param.value);
        }
        return *this;
    }

    template <typename Type>
    void
    writeValue(const std::string& id, const Type& value)
    {
        auto& writeTo = id.empty() ? json_ : json_[id];
        if constexpr (is_parameter_set<Type>::value || is_parameter_set_ref<Type>::value)
        {
            JsonPopulator pop;
            const_cast<Type&>(value).configure(pop);
            if (pop.json_.empty())
                writeTo = json::object();
            else
                writeTo = pop.json_;
        }
        else if constexpr (std::is_enum_v<Type>)
            writeTo = EnumMap<Type>::convert(value);
        else if constexpr (is_angle<Type>::value)
            writeTo = value.degrees();
        else if constexpr (is_vector<Type>::value)
        {
            Configuration config;
            for (const auto& v : value)
            {
                JsonPopulator pop;
                pop.writeValue("", v);
                config.push_back(pop.getConfig());
            }
            writeTo = config;
        }
        else if constexpr (is_string_key_map<Type>::value)
        {
            Configuration config;
            for (const auto& [key, value] : value)
            {
                JsonPopulator pop;
                pop.writeValue("", value);
                config["key"] = pop.getConfig();
            }
            writeTo = config;
        }
        else if constexpr (is_eigen_vector<Type>::value)
        {
            std::vector<typename Type::Scalar> vec(value.data(), value.data() + value.size());
            JsonPopulator pop;
            pop.writeValue("", vec);
            writeTo = pop.getConfig();
        }
        else if constexpr (is_optional<Type>::value)
        {
            if (value)
            {
                JsonPopulator pop;
                pop.writeValue("", *value);
                writeTo = pop.getConfig();
            }
            else
            {
                writeTo = nullptr;
            }
        }
        else
        {
            writeTo = value;
        }
    }

    template <typename Type,
              std::enable_if_t<is_parameter_set<Type>::value, int>  = 0>
    JsonPopulator&
    operator&(const Type& param)
    {
        const_cast<Type&>(param).configure(*this);
        return *this;
    }

    std::string
    getString() const;

    void
    toFile(const std::string& path) const;

    Configuration
    getConfig() const
    {
        return json_;
    }

    template <class... Objects>
    static JsonPopulator
    populate()
    {
        JsonPopulator pop;
        (pop.populateOne<Objects>(), ...);
        return pop;
    }

    template <typename Object>
    void
    populateOne()
    {
        if constexpr (is_configurable_object<Object>::value)
        {
            JsonPopulator pop;
            Object obj;
            obj.parse(pop);
            if constexpr (has_configure_params<Object>::value)
            {
                obj.configureParams(pop);
            }
            json_[Object::typeId] = pop.getConfig();
        }
        else if constexpr (is_static_factory<Object>::value)
        {
            auto pop = populateContainer<Object>();
            json_[Object::typeId] = pop.getConfig();
        }
        else if constexpr (is_static_helper<Object>::value)
        {
            auto pop = populateContainer<Object>();
            json_.merge_patch(pop.getConfig());
        }
        else if constexpr (is_parameter_set<Object>::value)
        {
            JsonPopulator pop;
            Object obj;
            obj.configure(pop);
            json_ = pop.getConfig();
        }
        else
        {
            // Not a configuratble object, but it can still be created.
            json_[Object::typeId] = Configuration::object();
        }
    }

    template <typename Container>
    struct populate_from_container
    {
        template <typename... Args>
        static JsonPopulator
        populateHelper(std::tuple<Args...>)
        {
            return JsonPopulator::populate<Args...>();
        }

        static JsonPopulator
        doPopulate()
        {
            return populateHelper(typename Container::Types{});
        }
    };

    template <typename Container>
    static JsonPopulator
    populateContainer()
    {
        return populate_from_container<Container>::doPopulate();
    }

private:
    json json_;
};

#endif /* UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_ */
