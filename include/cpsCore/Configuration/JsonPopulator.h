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
        if constexpr (is_parameter_set<Type>::value || is_parameter_set_ref<Type>::value)
        {
            JsonPopulator pop;
            const_cast<Type&>(value).configure(pop);
            json_[id] = pop.json_;
        }
        else if constexpr (std::is_enum_v<Type>)
            json_[id] = EnumMap<Type>::convert(value);
        else if constexpr  (is_angle<Type>::value)
        {
            json_[id] = value.degrees();
        }
        else
        {
            json_[id] = value;
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
    void
    populate()
    {
        (populateOne<Objects>(),...);
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
        else
        {
            CPSLOG_ERROR << "Type not supported";
        }
    }

    template <typename Container>
    struct populate_from_container
    {
        template <typename... Args>
        static JsonPopulator populateHelper(std::tuple<Args...>)
        {
            JsonPopulator pop;
            pop.populate<Args...>();
            return pop;
        }

        static JsonPopulator doPopulate()
        {
            return populateHelper(typename Container::Types{});
        }
    };

    template<typename Container>
    static JsonPopulator
    populateContainer()
    {
        return populate_from_container<Container>::doPopulate();
    }

private:
    json json_;
};

#endif /* UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_ */
