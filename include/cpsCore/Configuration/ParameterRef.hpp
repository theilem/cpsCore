/*
 * ParameterRef.h
 *
 *  Created on: Jun 28, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_PARAMETERREF_H_
#define UAVAP_CORE_PROPERTYMAPPER_PARAMETERREF_H_
#include <string>



template<typename Type>
struct ParameterRef
{
	using ValueType = Type;

	inline
	ParameterRef(ValueType& ref, const ValueType& def, const std::string& name, bool man) :
			value(ref), id(name), mandatory(man)
	{
		value = def;
	}

	inline
	ParameterRef(ValueType& ref, const std::string& name, bool man) :
			value(ref), id(name), mandatory(man)
	{
	}

	inline const Type&
	operator()() const
	{
		return value;
	}

	inline void
	setValue(const Type& val)
	{
		value = val;
	}

	Type& value;
	const std::string id;
	const bool mandatory;

};

template<typename Type>
struct is_parameter_ref : std::false_type
{
};

template<typename Type>
struct is_parameter_ref<ParameterRef<Type>> : std::true_type
{
};



#endif /* UAVAP_CORE_PROPERTYMAPPER_PARAMETERREF_H_ */
