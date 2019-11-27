/*
 * JsonPopulator.h
 *
 *  Created on: Jul 23, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_
#define UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_

#include "cpsCore/Configuration/ConfigurableObject.hpp"

class JsonPopulator
{
public:

	JsonPopulator();

	JsonPopulator(std::ofstream& file);

	~JsonPopulator();

	template<typename Type>
	std::enable_if_t<
			!is_parameter_set<typename Type::ValueType>::value
			&& !is_configurable_object<typename Type::ValueType>::value, JsonPopulator>&
	operator&(Type& param);

	template<typename Type>
	std::enable_if_t<(is_parameter_set<typename Type::ValueType>::value), JsonPopulator>&
	operator&(Type& param);

	template<typename Type>
	std::enable_if_t<(is_configurable_object<typename Type::ValueType>::value), JsonPopulator>&
	operator&(Type& param);

	std::string
	getString() const;

	void
	addTabs();

	void
	indent();

	void
	outdent();

	template<typename Type>
	JsonPopulator&
	operator<<(const Type& text);

	template<class ... Objects>
	void
	populate();

	template<template <class...Args> class Container, class ... Objects>
	inline void
	populateContainer(const Container<Objects...>& container);

private:


	template<class Obj, std::enable_if_t<is_configurable_object<Obj>::value, int> = 0>
	void
	populateImpl();

	template<class Obj, std::enable_if_t<!is_configurable_object<Obj>::value, int> = 0>
	void
	populateImpl();


	template<typename Type>
	std::enable_if_t<is_string<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<std::is_enum<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<is_angle<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<
			!std::is_enum<Type>::value && !is_string<Type>::value && !is_angle<Type>::value,
			JsonPopulator>&
	writeValue(const Type& value);

	std::stringstream stringStream_;
	std::ostream jsonString_;

	bool firstElement_ = true;
	int tabCounter_ = 0;
};

template<typename Type>
inline std::enable_if_t<
		!is_parameter_set<typename Type::ValueType>::value
		&& !is_configurable_object<typename Type::ValueType>::value, JsonPopulator>&
JsonPopulator::operator&(Type& param)
{
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	addTabs();

	firstElement_ = false;
	jsonString_ << "\"" << param.id << "\":";
	this->template writeValue<typename Type::ValueType>(param.value);

	return *this;
}

template<typename Type>
inline std::enable_if_t<(is_parameter_set<typename Type::ValueType>::value), JsonPopulator>&
JsonPopulator::operator&(Type& param)
{
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	addTabs();

	jsonString_ << "\"" << param.id << "\":{" << std::endl;

	firstElement_ = true;
	tabCounter_++;
	param.value.configure(*this);
	firstElement_ = false;
	tabCounter_--;

	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
	return *this;
}

template<typename Type>
inline std::enable_if_t<(is_configurable_object<typename Type::ValueType>::value),
		JsonPopulator>&
JsonPopulator::operator&(Type& param)
{
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	addTabs();

	jsonString_ << "\"" << param.id << "\":{" << std::endl;

	firstElement_ = true;
	tabCounter_++;
	param.value.configureParams(*this);
	firstElement_ = false;
	tabCounter_--;

	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
	return *this;
}

template<typename Type>
inline std::enable_if_t<is_string<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	jsonString_ << "\"" << value << "\"";
	return *this;
}

template<typename Type>
inline std::enable_if_t<std::is_enum<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	jsonString_ << EnumMap<Type>::convert(value);
	return *this;
}

template<typename Type>
inline std::enable_if_t<is_angle<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	jsonString_ << value.degrees();
	return *this;
}

template<typename Type>
inline std::enable_if_t<
		!std::is_enum<Type>::value && !is_string<Type>::value && !is_angle<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	jsonString_ << value;
	return *this;
}

template<typename Type>
inline JsonPopulator&
JsonPopulator::operator<<(const Type& text)
{
	jsonString_ << text;
	return *this;
}

template<class ... Objects>
inline void
JsonPopulator::populate()
{
	(populateImpl<Objects>(),...);
}

template<template <class...Args> class Container, class ... Objects>
inline void
JsonPopulator::populateContainer(const Container<Objects...>& container)
{
	(populateImpl<Objects>(),...);
}

template<class Obj, std::enable_if_t<is_configurable_object<Obj>::value, int>>
inline void
JsonPopulator::populateImpl()
{
	//Is configurable Object
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	addTabs();
	jsonString_ << "\"" << Obj::typeId << "\"" << ":{" << std::endl;
//	addTabs();
	indent();
	Obj p;
	firstElement_ = true;
	p.configureParams(*this);
	outdent();
	firstElement_ = false;

	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
}

template<class Obj, std::enable_if_t<!is_configurable_object<Obj>::value, int>>
inline void
JsonPopulator::populateImpl()
{
	//Is not configurable, just add type
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	addTabs();
	firstElement_ = false;
	jsonString_ << "\"" << Obj::typeId << "\"" << ":{" << std::endl;
	addTabs();
	jsonString_ << "}";
}

#endif /* UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_ */
