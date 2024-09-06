/*
 * Parameter.h
 *
 *  Created on: Jun 24, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_PARAMETER_H_
#define UAVAP_CORE_PROPERTYMAPPER_PARAMETER_H_

#include <string>
#include <iostream>

template<typename Type>
struct Parameter
{
	using ValueType = Type;

	Parameter<Type>&
	operator=(const Parameter<Type>& other)
	{
		value = other.value;
		return *this;
	}

	Parameter<Type>&
	operator=(const Type& val)
	{
		value = val;
		return *this;
	}

	inline const Type&
	operator()() const
	{
		return value;
	}

	inline Type&
	operator()()
	{
		return value;
	}

	bool
	operator==(const Parameter<Type>& other) const
	{
		return value == other.value;
	}

	inline void
	setValue(const Type& val)
	{
		value = val;
	}

	Type value;
	const std::string id;
	const bool mandatory;

};

class ParameterPrinter
{
public:
	explicit
	ParameterPrinter(std::ostream& os) :
		os_(os)
	{
	}

	template <typename Type>
	ParameterPrinter&
	operator&(const Parameter<Type>& value)
	{
		os_ << value.id << ": " << value.value << std::endl;
		return *this;
	}

	template <typename ParameterSet>
	std::ostream&
	operator<<(const ParameterSet& set)
	{
		return print(set);
	}

	template <typename ParameterSet>
	std::ostream&
	print(const ParameterSet& set)
	{
		const_cast<ParameterSet&>(set).configure(*this);
		return os_;
	}

private:
	std::ostream& os_;
};

template<typename, typename = void>
struct is_parameter_set : std::false_type {};

template<typename T>
struct is_parameter_set<T, std::void_t<decltype(std::declval<T>().configure(std::declval<int&>()))>> : std::true_type {};


template<typename Type>
struct is_parameter : public std::false_type
{
};

template<typename Type>
struct is_parameter<Parameter<Type>> : public std::true_type
{
};

template<typename Type>
using enable_if_is_parameter = typename std::enable_if<is_parameter<Type>::value, Type>::type;
template<typename Type>
using enable_if_not_is_parameter = typename std::enable_if<!is_parameter<Type>::value, Type>::type;

template<typename Type>
using enable_if_is_parameter_set = typename std::enable_if<is_parameter_set<Type>::value, Type>::type;
template<typename Type>
using enable_if_not_is_parameter_set = typename std::enable_if<!(is_parameter_set<Type>::value), Type>::type;


template <typename Type, typename ParameterSetType = std::enable_if_t<is_parameter_set<Type>::value>>
std::ostream&
operator<<(std::ostream& os, const Type& value)
{
	ParameterPrinter printer(os);
	printer << value;
	return os;
}


#endif /* UAVAP_CORE_PROPERTYMAPPER_PARAMETER_H_ */
