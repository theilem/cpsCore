/*
 * PropertyMapper.hpp
 *
 *  Created on: Jun 17, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_PROPERTYMAPPER_H_
#define UAVAP_CORE_PROPERTYMAPPER_PROPERTYMAPPER_H_

#include <type_traits>

#include <Eigen/Core>
#include <boost/optional.hpp>

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

template<typename Configuration>
class PropertyMapper
{
public:
	PropertyMapper(const Configuration& p);

	template<typename PODType>
	bool
	add(const std::string& key,
		typename std::enable_if<std::is_pod<PODType>::value, PODType>::type& val,
		bool mandatory);

	template<typename Type>
	bool
	add(const std::string& key, typename std::enable_if<!std::is_pod<Type>::value, Type>::type& val,
		bool mandatory);

	template<typename T>
	bool
	addVector(const std::string& key,
			  std::vector<enable_if_is_parameter_set<typename T::value_type>>& val, bool mandatory);

	template<typename T>
	bool
	addVector(const std::string& key,
			  std::vector<enable_if_not_is_parameter_set<typename T::value_type>>& val,
			  bool mandatory);

	template<typename T>
	bool
	addMap(const std::string& key,
		   std::map<std::string, enable_if_is_parameter_set<typename T::value_type::second_type>>& val, bool mandatory);

	template<typename T>
	bool
	addMap(const std::string& key,
		   std::map<std::string, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
		   bool mandatory);

	template<typename T>
	bool
	addMap(const std::string& key,
		   std::unordered_map<std::string, enable_if_is_parameter_set<typename T::value_type::second_type>>& val,
		   bool mandatory);

	template<typename T>
	bool
	addMap(const std::string& key,
		   std::unordered_map<std::string, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
		   bool mandatory);

	bool
	addVector(const std::string& key, std::vector<Configuration>& val, bool mandatory);

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

	template<typename Type>
	bool
	add(const std::string& key, Eigen::Matrix<Type, Eigen::Dynamic, 1>& val, bool mandatory);

	template<typename Type>
	bool
	add(const std::string& key, Eigen::Array<Type, Eigen::Dynamic, 1>& val, bool mandatory);

	template<typename Type>
	bool
	addAngle(const std::string& key, Angle<Type>& val, bool mandatory);

	template<typename Type>
	bool
	addOptional(const std::string& key, Optional<Type>& val, bool mandatory);

	template<typename Enum>
	bool
	addEnum(const std::string& key, Enum& e, bool mandatory);

	template<typename Enum>
	bool
	addEnumVector(const std::string& key, std::vector<Enum>& e, bool mandatory);

	template<typename Type, int rows, int cols, int options>
	bool
	addEigen(const std::string& key, Eigen::Matrix<Type, rows, cols, options>& val, bool mandatory);

	template<typename Param>
	bool
	operator&(Param& param);

	bool
	map();

	PropertyMapper
	getChild(const std::string& key, bool mandatory);

	bool
	isEmpty() const;

protected:

	const Configuration& p_;
	bool mandatoryCheck_;

private:
	template<typename Type>
	struct is_special_param
	{
		template<typename _1>
		static char
		chk(
				decltype(std::declval<PropertyMapper<Configuration>>().add(std::string(), std::declval<_1&>(), true)));

		template<typename>
		static int
		chk(...);

		static constexpr bool value = sizeof(chk<Type>(0)) == sizeof(char);
	};

	template<typename Type>
	bool
	addSpecific(const std::string& key, Type& val, bool mandatory);

};

template<typename Config>
template<typename PODType>
inline bool
PropertyMapper<Config>::add(const std::string& key,
							typename std::enable_if<std::is_pod<PODType>::value, PODType>::type& val, bool mandatory)
{
	auto value = p_.template get_optional<PODType>(key);
	if (value)
	{
		CPSLOG_TRACE << "Property " << key << " = " << *value;
		val = *value;
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename Type>
inline bool
PropertyMapper<Config>::add(const std::string& key,
							typename std::enable_if<!std::is_pod<Type>::value, Type>::type& val, bool mandatory)
{
	if (key.empty())
	{
		bool success = val.configure(p_);
		if (mandatory)
			mandatoryCheck_ &= success;

		return success;
	}
	else
	{
		Config subtree;
		add(key, subtree, false);
		if (!subtree.empty())
		{
			if (val.configure(subtree))
				return true;
		}
		if (mandatory)
		{
			CPSLOG_ERROR << "PM: mandatory " << key << " missing";
			mandatoryCheck_ = false;
		}
		return false;
	}
}

template<typename Config>
template<typename T>
inline bool
PropertyMapper<Config>::addVector(const std::string& key,
								  std::vector<enable_if_is_parameter_set<typename T::value_type>>& val, bool mandatory)
{
	val.clear();
	boost::optional<const Config&> value;
	if (key.empty())
		value = p_;
	else
		value = p_.get_child_optional(key);
	if (value)
	{
		const Config& config = *value;
		for (auto& it : config)
		{
			PropertyMapper<Config> pm(it.second);
			val.push_back(typename T::value_type());
			val.back().configure(pm);
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename T>
inline bool
PropertyMapper<Config>::addVector(const std::string& key,
								  std::vector<enable_if_not_is_parameter_set<typename T::value_type>>& val,
								  bool mandatory)
{
	val.clear();
	boost::optional<const Config&> value;
	if (key.empty())
		value = p_;
	else
		value = p_.get_child_optional(key);
	if (value)
	{
		const Config& config = *value;
		for (auto& it : config)
		{
			val.push_back(it.second.template get_value<typename T::value_type>());
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename Type>
inline bool
PropertyMapper<Config>::add(const std::string& key, Eigen::Matrix<Type, Eigen::Dynamic, 1>& val,
							bool mandatory)
{
	auto value = p_.get_child_optional(key);
	if (value)
	{
		Config child = *value;
		Eigen::Matrix<Type, -1, 1> values(child.size(), 1);
		int k = 0;
		for (auto& it : child)
		{
			values[k++] = it.second.template get_value<Type>();
		}
		val = values;
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename Type>
inline bool
PropertyMapper<Config>::add(const std::string& key, Eigen::Array<Type, Eigen::Dynamic, 1>& val,
							bool mandatory)
{
	val =
			{};
	auto value = p_.get_child_optional(key);
	if (value)
	{
		Config child = *value;
		Eigen::Matrix<Type, -1, 1> values(child.size(), 1);
		int k = 0;
		for (auto& it : child)
		{
			values[k++] = it.second.template get_value<Type>();
		}
		val = values.array();
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename Enum>
inline bool
PropertyMapper<Config>::addEnum(const std::string& key, Enum& e, bool mandatory)
{
	auto value = p_.template get_optional<std::string>(key);
	if (value)
	{
		CPSLOG_TRACE << "Property " << key << " = " << *value;
		e = EnumMap<Enum>::convert(*value);
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename Enum>
inline bool
PropertyMapper<Config>::addEnumVector(const std::string& key, std::vector<Enum>& e, bool mandatory)
{
	e.clear();
	auto value = p_.get_child_optional(key);
	if (value)
	{
		Config child = *value;
		for (auto& it : child)
		{
			e.push_back(EnumMap<Enum>::convert(it.second.template get_value<std::string>()));
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
PropertyMapper<Config>::PropertyMapper(const Config& p) :
		p_(p), mandatoryCheck_(true)
{
}

template<typename Config>
bool
PropertyMapper<Config>::add(const std::string& key, std::string& val, bool mandatory)
{
	auto value = p_.template get_optional<std::string>(key);
	if (value)
	{
		CPSLOG_TRACE << "Property " << key << " = " << *value;
		val = *value;
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
bool
PropertyMapper<Config>::add(const std::string& key, Duration& val, bool mandatory)
{
	auto value = p_.template get_optional<int>(key);
	if (value)
	{
		CPSLOG_TRACE << "Property " << key << " = " << *value;
		val = Milliseconds(*value);
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
bool
PropertyMapper<Config>::add(const std::string& key, Config& val, bool mandatory)
{

	//See if it is a path that contains another configuration file
	auto path = p_.template get_optional<std::string>(key);
	if (path && !path->empty())
	{
	}
	else
	{
		auto value = p_.get_child_optional(key);
		if (value)
		{
			val = *value;
			return true;
		}
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
bool
PropertyMapper<Config>::addVector(const std::string& key, std::vector<Config>& val, bool mandatory)
{
	val.clear();
	auto value = p_.get_child_optional(key);
	if (value)
	{
		Config child = *value;
		for (auto& it : child)
		{
			val.push_back(it.second);
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
bool
PropertyMapper<Config>::map()
{
	//Do some error printouts
	return mandatoryCheck_;
}

template<typename Config>
bool
PropertyMapper<Config>::add(const std::string& key, Vector3& val, bool mandatory)
{
	std::vector<double> vec;
	if (!this->template addVector<std::vector<double>>(key, vec, mandatory))
		return false;

	if (vec.size() == 3)
	{
		val = Vector3(vec[0], vec[1], vec[2]);
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: Vector " << key << " does not have 3 values, only " << vec.size();
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename Type, int rows, int cols, int options>
bool
PropertyMapper<Config>::addEigen(const std::string& key, Eigen::Matrix<Type, rows, cols, options>& val, bool mandatory)
{
	std::vector<Type> vec;
	if (!this->template addVector<std::vector<Type>>(key, vec, mandatory))
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

template<typename Config>
bool
PropertyMapper<Config>::add(const std::string& key, Vector2& val, bool mandatory)
{
	std::vector<double> vec;
	if (!this->template addVector<std::vector<double>>(key, vec, mandatory))
		return false;

	if (vec.size() == 2)
	{
		val = Vector2(vec[0], vec[1]);
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: Vector " << key << " does not have 2 values, only " << vec.size();
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
PropertyMapper<Config>
PropertyMapper<Config>::getChild(const std::string& key, bool mandatory)
{

	auto value = p_.get_child_optional(key);
	if (value)
	{
		PropertyMapper<Config> p(*value);
		return p;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return PropertyMapper(Config());
}

template<typename Config>
bool
PropertyMapper<Config>::isEmpty() const
{
	return p_.empty();
}

template<typename Config>
template<typename Param>
bool
PropertyMapper<Config>::operator&(Param& param)
{
	if constexpr (is_parameter_set<typename Param::ValueType>::value)
	{
		auto pm = getChild(param.id, param.mandatory);
		if (!pm.isEmpty())
		{
			param.value.configure(pm);
			return pm.map();
		}
		return false;
	}
	else if constexpr (is_parameter_set_ref<typename Param::ValueType>::value)
	{
		auto pm = getChild(param.id, param.mandatory);
		if (!pm.isEmpty())
		{
			configure(pm, param.value);
			return pm.map();
		}
		return false;
	}
	else
		return addSpecific<typename Param::ValueType>(param.id, param.value, param.mandatory);
}

template<typename Config>
template<typename Type>
bool
PropertyMapper<Config>::addSpecific(const std::string& key, Type& val, bool mandatory)
{
	if constexpr (is_special_param<Type>::value)
		return add(key, val, mandatory);
	else if constexpr (std::is_enum<Type>::value)
		return addEnum(key, val, mandatory);
	else if constexpr (is_vector<Type>::value)
		return addVector<Type>(key, val, mandatory);
	else if constexpr (is_string_key_map<Type>::value)
		return addMap<Type>(key, val, mandatory);
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

template<typename Config>
template<typename Type>
inline bool
PropertyMapper<Config>::addAngle(const std::string& key, Angle<Type>& val, bool mandatory)
{
	Type v;
	auto b = this->template add<Type>(key, v, mandatory);
	if (!b)
		return false;
	val = v;

	return true;
}

template<typename Config>
template<typename Type>
inline bool
PropertyMapper<Config>::addOptional(const std::string& key, Optional<Type>& val, bool mandatory)
{
	Type v;
	if (this->addSpecific<Type>(key, v, mandatory))
	{
		val = v;
		return true;
	}

	val = std::nullopt;
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename T>
bool
PropertyMapper<Config>::addMap(const std::string& key,
							   std::map<std::string, enable_if_is_parameter_set<typename T::value_type::second_type>>& val,
							   bool mandatory)
{
	val.clear();
	boost::optional<const Config&> value;
	if (key.empty())
		value = p_;
	else
		value = p_.get_child_optional(key);
	if (value)
	{
		const Config& config = *value;
		for (auto& it : config)
		{
			PropertyMapper<Config> pm(it.second);
			auto i = val.insert(std::make_pair(it.first, typename T::value_type::second_type()));
			i.first->second.configure(pm);
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename T>
bool
PropertyMapper<Config>::addMap(const std::string& key,
							   std::map<std::string, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
							   bool mandatory)
{
	val.clear();
	boost::optional<const Config&> value;
	if (key.empty())
		value = p_;
	else
		value = p_.get_child_optional(key);
	if (value)
	{
		const Config& config = *value;
		for (auto& it : config)
		{
			val.insert(std::make_pair(it.first, it.second.template get_value<typename T::value_type::second_type>()));
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename T>
bool
PropertyMapper<Config>::addMap(const std::string& key,
							   std::unordered_map<std::string, enable_if_is_parameter_set<typename T::value_type::second_type>>& val,
							   bool mandatory)
{
	val.clear();
	boost::optional<const Config&> value;
	if (key.empty())
		value = p_;
	else
		value = p_.get_child_optional(key);
	if (value)
	{
		const Config& config = *value;
		for (auto& it : config)
		{
			PropertyMapper<Config> pm(it.second);
			auto i = val.insert(std::make_pair(it.first, typename T::value_type::second_type()));
			i.first->second.configure(pm);
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}

template<typename Config>
template<typename T>
bool
PropertyMapper<Config>::addMap(const std::string& key,
							   std::unordered_map<std::string, enable_if_not_is_parameter_set<typename T::value_type::second_type>>& val,
							   bool mandatory)
{
	val.clear();
	boost::optional<const Config&> value;
	if (key.empty())
		value = p_;
	else
		value = p_.get_child_optional(key);
	if (value)
	{
		const Config& config = *value;
		for (auto& it : config)
		{
			val.insert(std::make_pair(it.first, it.second.template get_value<typename T::value_type::second_type>()));
		}
		return true;
	}
	if (mandatory)
	{
		CPSLOG_ERROR << "PM: mandatory " << key << " missing";
		mandatoryCheck_ = false;
	}
	return false;
}


#endif /* UAVAP_CORE_PROPERTYMAPPER_PROPERTYMAPPER_H_ */
