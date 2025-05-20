/*
 * PropertyMapper.hpp
 *
 * Created on: Jun 17, 2017
 *     Author: mircot
 *
 * Description:
 * ------------
 * PropertyMapper is a utility class used to map configuration parameters
 * (e.g., parsed from a JSON file) to C++ variables and data structures.
 * It handles basic POD types, STL containers (vectors, maps), Eigen types,
 * and user-defined types that implement a `configure(PropertyMapper&)` method.
 *
 *
 * Functionality:
 * --------------
 * - Extracts and assigns values from a Configuration object
 * - Supports nested configurations and recursive mapping
 * - Supports mandatory/optional field enforcement
 * - Supports enumeration mapping via EnumMap
 * - Supports Eigen vector and matrix types
 * - Type-safe using C++17 SFINAE utilities (e.g., enable_if)
 *
 * Example usage:
 *  Configuration config = ...; // parsed from file
 *  PropertyMapper pm(config);
 *  double gain;
 *  pm.add("controller.gain", gain, true);
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_PROPERTYMAPPER_H_
#define UAVAP_CORE_PROPERTYMAPPER_PROPERTYMAPPER_H_

#include <type_traits>

#include <Eigen/Core>

#include "Configuration.hpp"
#include "cpsCore/Configuration/Parameter.hpp"
#include "cpsCore/Utilities/Optional.hpp"
#include "cpsCore/Configuration/ParameterRef.hpp"
#include "cpsCore/Configuration/TypeTraits.hpp"
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/Angle.hpp"
#include "cpsCore/Utilities/EnumMap.hpp"
#include "cpsCore/Utilities/Time.hpp"
#include "cpsCore/Utilities/LinearAlgebra.h"
#include "cpsCore/Utilities/TypeTraits.hpp"

class PropertyMapper
{
public:
    explicit
    PropertyMapper(const Configuration& p) :
        p_(p), mandatoryCheck_(true), empty_(p.empty())
    {
    }

    template <typename PODType>
    bool
    add(const std::string& key, std::enable_if_t<std::is_pod_v<PODType>, PODType>& val, bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end() && !it->is_null())
        {
            try
            {
                val = it->get<PODType>();
                CPSLOG_TRACE << "Property " << key << " = " << val;
                return true;
            }
            catch (const Configuration::exception& e)
            {
            }
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename Type>
    bool
    add(const std::string& key, std::enable_if_t<!std::is_pod_v<Type>, Type>& val, bool mandatory)
    {
        auto pm = getChild(key, mandatory);
        if (!pm.isEmpty())
        {
            if (val.configure(pm))
                return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addVector(const std::string& key, std::vector<T>& val, bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end() && it->is_array())
        {
            val.clear();
            for (auto& value : *it)
            {
                if constexpr (std::is_same_v<T, Configuration>)
                {
                    val.push_back(value);
                }
                else if constexpr (is_parameter_set<T>::value)
                {
                    PropertyMapper pm(value);
                    val.emplace_back();
                    val.back().configure(pm);
                }
                else if constexpr (std::is_enum_v<T>)
                {
                    val.push_back(EnumMap<T>::convert(value.get<std::string>()));
                }
                else if constexpr (is_eigen<T>::value)
                {
                    if (value.is_array())
                    {
                        auto vec = value.get<std::vector<typename T::Scalar>>();
                        val.emplace_back();
                        val.back() = Eigen::Map<T>(vec.data(), vec.size());
                    }
                    else
                    {
                        CPSLOG_ERROR << "Property " << key << " is not a vector";
                        return false;
                    }
                }
                else
                    val.push_back(value.get<T>());
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addMap(const std::string& key,
           std::map<std::string, enable_if_is_parameter_set<typename T::value_type::second_type>>& val, bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                auto i = val.insert(std::make_pair(key, typename T::value_type::second_type()));
                i.first->second.configure(pm);
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addMap(const std::string& key,
           std::map<std::string, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
           bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                val.insert(std::make_pair(key, value.get<typename T::value_type::second_type>()));
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addMap(const std::string& key,
           std::unordered_map<std::string, enable_if_is_parameter_set<typename T::value_type::second_type>>& val,
           bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                auto i = val.insert(std::make_pair(key, typename T::value_type::second_type()));
                i.first->second.configure(pm);
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addMap(const std::string& key,
           std::unordered_map<std::string, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
           bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                val.insert(
                    std::make_pair(key, value.get<typename T::value_type::second_type>()));
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addEnumMap(const std::string& key,
               std::map<typename T::key_type, enable_if_is_parameter_set<typename T::value_type::second_type>>& val,
               bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                auto i = val.insert(std::make_pair(EnumMap<typename T::key_type>::convert(key),
                                                   typename T::value_type::second_type()));
                i.first->second.configure(pm);
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addEnumMap(const std::string& key,
               std::map<typename T::key_type, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
               bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                auto i = val.insert(std::make_pair(EnumMap<typename T::key_type>::convert(key),
                                                   value.get<typename T::value_type::second_type>()));
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addEnumMap(const std::string& key,
               std::unordered_map<typename T::key_type, enable_if_is_parameter_set<typename T::value_type::second_type>>
               & val,
               bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                auto i = val.insert(std::make_pair(EnumMap<typename T::key_type>::convert(key),
                                                   typename T::value_type::second_type()));
                i.first->second.configure(pm);
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename T>
    bool
    addEnumMap(const std::string& key,
               std::unordered_map<typename T::key_type, enable_if_not_is_parameter_set<typename
                                      T::value_type::second_type>>& val,
               bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end())
        {
            val.clear();
            for (auto& [key, value] : it->items())
            {
                PropertyMapper pm(value);
                val.insert(std::make_pair(EnumMap<typename T::key_type>::convert(key),
                                          value.get<typename T::value_type::second_type>()));
            }
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    bool
    add(const std::string& key, Duration& val, bool mandatory);

    bool
    add(const std::string& key, std::string& val, bool mandatory);

    bool
    add(const std::string& key, Vector3& val, bool mandatory);

    bool
    add(const std::string& key, Vector2& val, bool mandatory);

    bool
    add(const std::string& key, Configuration& val, bool mandatory);

    template <typename Type>
    bool
    add(const std::string& key, Eigen::Matrix<Type, Eigen::Dynamic, 1>& val, bool mandatory)
    {
        std::vector<Type> vec;
        if (!this->addVector(key, vec, mandatory))
            return false;

        Eigen::Matrix<Type, -1, 1> values(vec.size(), 1);
        for (size_t i = 0; i < vec.size(); ++i)
        {
            values(i) = vec[i];
        }
        val = values;
        return true;
    }

    template <typename Type>
    bool
    add(const std::string& key, Eigen::Array<Type, Eigen::Dynamic, 1>& val, bool mandatory)
    {
        std::vector<Type> vec;
        if (!this->addVector(key, vec, mandatory))
            return false;

        Eigen::Array<Type, -1, 1> values(vec.size(), 1);
        for (size_t i = 0; i < vec.size(); ++i)
        {
            values(i) = vec[i];
        }
        val = values;
        return true;
    }

    template <typename Type>
    bool
    addAngle(const std::string& key, Angle<Type>& val, bool mandatory)
    {
        Type v;
        if (!this->add<Type>(key, v, mandatory))
            return false;
        val = v;
        return true;
    }

    template <typename Type>
    bool
    addOptional(const std::string& key, Optional<Type>& val, bool mandatory)
    {
        Type v;
        if (this->addSpecific<Type>(key, v, mandatory))
        {
            val = v;
            return true;
        }
        val = std::nullopt;
        return false;
    }

    template <typename Enum>
    bool
    addEnum(const std::string& key, Enum& e, bool mandatory)
    {
        if (auto it = p_.find(key); it != p_.end() && it->is_string())
        {
            auto str = it->get<std::string>();
            CPSLOG_TRACE << "Property " << key << " = " << str;
            e = EnumMap<Enum>::convert(str);
            return true;
        }
        mandatoryCheck(key, mandatory);
        return false;
    }

    template <typename Type, int rows, int cols, int options>
    bool
    addEigen(const std::string& key, Eigen::Matrix<Type, rows, cols, options>& val, bool mandatory)
    {
        std::vector<Type> vec;
        if (!this->addVector(key, vec, mandatory))
            return false;

        if (vec.size() == rows * cols)
        {
            val = Eigen::Matrix<Type, rows, cols, options>(vec.data());
            return true;
        }
        if (mandatory)
        {
            CPSLOG_ERROR << "PM: Vector " << key << " does not have " << rows * cols << " values, only " << vec.size();
            mandatoryCheck_ = false;
        }
        return false;
    }

    template <typename Param>
    bool
    operator&(Param& param)
    {
        static_assert(is_parameter<Param>::value || is_parameter_ref<Param>::value,
                      "Cannot handle non parameter objects");

        if constexpr (is_parameter_set_ref<typename Param::ValueType>::value)
        {
            auto pm = getChild(param.id, param.mandatory);
            if (!pm.isEmpty())
            {
                configure(pm, param.value);
                return pm.map();
            }
            return false;
        }
        else if constexpr (is_parameter_set<typename Param::ValueType>::value)
        {
            auto pm = getChild(param.id, param.mandatory);
            if (!pm.isEmpty())
            {
                param.value.configure(pm);
                return pm.map();
            }
            return false;
        }
        else
            return addSpecific<typename Param::ValueType>(param.id, param.value, param.mandatory);
    }

    bool
    map() const
    {
        return mandatoryCheck_;
    }

    PropertyMapper
    getChild(const std::string& key, bool mandatory)
    {
        if (auto value = p_.find(key); value != p_.end() && value->is_object())
        {
            return PropertyMapper(*value);
        }
        if (mandatory)
        {
            CPSLOG_ERROR << "PM: mandatory " << key << " missing";
            mandatoryCheck_ = false;
        }
        return PropertyMapper(Configuration());
    }

    bool
    isEmpty() const
    {
        return empty_ || p_.empty();
    }

protected:
    const Configuration& p_;
    bool mandatoryCheck_;
    bool empty_;

private:
    template <typename Type>
    bool
    addSpecific(const std::string& key, Type& val, bool mandatory);

    void
    mandatoryCheck(const std::string& key, bool mandatory)
    {
        if (mandatory)
        {
            CPSLOG_ERROR << "PM: mandatory " << key << " missing";
            mandatoryCheck_ = false;
        }
    }
};

template <typename T, typename = void>
struct is_special_param : std::false_type
{
};

template <typename T>
struct is_special_param<T,
                        std::void_t<decltype(
                            std::declval<PropertyMapper>().add(std::declval<std::string>(), std::declval<T&>(),
                                                               true)
                        )>> : std::true_type
{
};


template <typename Type>
inline bool
PropertyMapper::addSpecific(const std::string& key, Type& val, bool mandatory)
{
    if constexpr (is_special_param<Type>::value)
        return add(key, val, mandatory);
    else if constexpr (std::is_enum_v<Type>)
        return addEnum(key, val, mandatory);
    else if constexpr (is_vector<Type>::value)
        return addVector(key, val, mandatory);
    else if constexpr (is_string_key_map<Type>::value)
        return addMap<Type>(key, val, mandatory);
    else if constexpr (is_enum_map<Type>::value)
        return addEnumMap<Type>(key, val, mandatory);
    else if constexpr (is_angle<Type>::value)
        return addAngle<typename Type::ValueType>(key, val, mandatory);
    else if constexpr (is_optional<Type>::value)
        return addOptional<typename Type::value_type>(key, val, mandatory);
    else if constexpr (is_eigen<Type>::value)
        return addEigen(key, val, mandatory);
    else if constexpr (is_parameter_set<Type>::value)
    {
        auto pm = getChild(key, mandatory);
        if (!pm.isEmpty())
        {
            val.configure(pm);
            return pm.map();
        }
        return false;
    }
    else if constexpr (is_parameter_set_ref<Type>::value)
    {
        auto pm = getChild(key, mandatory);
        if (!pm.isEmpty())
        {
            configure(pm, val);
            return pm.map();
        }
        return false;
    }
    else
        return add<Type>(key, val, mandatory);
}


#endif /* UAVAP_CORE_PROPERTYMAPPER_PROPERTYMAPPER_H_ */
