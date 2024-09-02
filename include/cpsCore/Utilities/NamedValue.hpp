/*
 * Parameter.h
 *
 *  Created on: Jun 24, 2019
 *      Author: mirco
 */

#ifndef CPSCORE_UTILITIES_NAMEDVALUE_HPP_
#define CPSCORE_UTILITIES_NAMEDVALUE_HPP_

#include <string>

template<typename Type>
struct NamedValue
{
	using ValueType = Type;

	NamedValue<Type>&
	operator=(const NamedValue<Type>& other)
	{
		value = other.value;
		return *this;
	}

	NamedValue<Type>&
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

	inline void
	setValue(const Type& val)
	{
		value = val;
	}

	Type value;
	const std::string id;

};

template<typename Type>
struct is_named_value_set
{
	template<typename _1>
	static char&
	chk(
			typename std::enable_if<
					std::is_same<void, decltype(std::declval<_1>().parse(std::declval<int&>()))>::value,
					int>::type);

	template<typename>
	static int&
	chk(...);

	static constexpr bool value = sizeof(chk<Type>(0)) == sizeof(char);
};

template<typename Type>
struct is_named_value : public std::false_type
{
};

template<typename Type>
struct is_named_value<NamedValue<Type>> : public std::true_type
{
};

template<typename Type>
using enable_if_is_named_value = typename std::enable_if<is_named_value<Type>::value, Type>::type;
template<typename Type>
using enable_if_not_is_named_value = typename std::enable_if<!is_named_value<Type>::value, Type>::type;

template<typename Type>
using enable_if_is_named_value_set = typename std::enable_if<is_named_value_set<Type>::value, Type>::type;
template<typename Type>
using enable_if_not_is_named_value_set = typename std::enable_if<!(is_named_value_set<Type>::value), Type>::type;

#endif /* CPSCORE_UTILITIES_NAMEDVALUE_HPP_ */
