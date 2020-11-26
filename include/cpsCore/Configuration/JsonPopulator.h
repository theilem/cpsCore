/*
 * JsonPopulator.h
 *
 *  Created on: Jul 23, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_
#define UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_

#include "cpsCore/Configuration/ConfigurableObject.hpp"
#include "cpsCore/Framework/StaticFactory.h"
#include "cpsCore/Framework/StaticHelper.h"
#include <fstream>

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

	template<template<class...Args> class Container, class ... Objects>
	inline void
	populateContainer(const Container<Objects...>& container);

	template<class Container>
	static int
	fromMain(int argc, char** argv);

private:


	template<class Obj, std::enable_if_t<is_configurable_object<Obj>::value, int> = 0>
	void
	populateImpl();

	template<class Obj, std::enable_if_t<is_static_factory<Obj>::value, int> = 0>
	void
	populateImpl();

	template<class Obj, std::enable_if_t<is_static_helper<Obj>::value, int> = 0>
	void
	populateImpl();

	template<class Obj, std::enable_if_t<
			!is_configurable_object<Obj>::value && !is_static_factory<Obj>::value &&
			!is_static_helper<Obj>::value, int> = 0>
	void
	populateImpl();


	template<class Obj, std::enable_if_t<
			has_configure_params<Obj>::value, int> = 0>
	void
	handleParams();


	template<class Obj, std::enable_if_t<
			!has_configure_params<Obj>::value, int> = 0>
	void
	handleParams();


	template<class Obj>
	bool
	populateImplBoolRet();


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
	std::enable_if_t<is_vector<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<is_string_key_map<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<is_optional<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<is_eigen<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<is_parameter_set<Type>::value, JsonPopulator>&
	writeValue(const Type& value);

	template<typename Type>
	std::enable_if_t<
			!std::is_enum<Type>::value && !is_string<Type>::value && !is_angle<Type>::value &&
			!is_vector<Type>::value && !is_parameter_set<Type>::value && !is_string_key_map<Type>::value &&
			!is_optional<Type>::value && !is_eigen<Type>::value,
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
	jsonString_ << "\"" << param.id << "\": ";
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

	jsonString_ << "\"" << param.id << "\": {" << std::endl;

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

	jsonString_ << "\"" << param.id << "\": {" << std::endl;

	firstElement_ = true;
	tabCounter_++;
	handleParams<typename Type::ValueType>();
//	param.value.configureParams(*this);
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
	jsonString_ << "\"" << EnumMap<Type>::convert(value) << "\"";
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
inline std::enable_if_t<is_eigen<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	std::vector<typename Type::value_type> vec(value.data(), value.data() + value.rows() * value.cols());
	return writeValue(vec);
}

template<typename Type>
inline std::enable_if_t<is_vector<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	jsonString_ << "[" << std::endl;
	indent();
	firstElement_ = true;
	for (const auto& it : value)
	{
		if (!firstElement_)
			jsonString_ << "," << std::endl;
		addTabs();
		writeValue(it);
		firstElement_ = false;
	}
	firstElement_ = false;

	outdent();
	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "]";
	return *this;
}


template<typename Type>
std::enable_if_t<is_string_key_map<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	jsonString_ << "{" << std::endl;
	indent();
	firstElement_ = true;
	for (const auto& it : value)
	{
		if (!firstElement_)
			jsonString_ << "," << std::endl;
		addTabs();
		jsonString_ << "\"" << it.first << "\": ";
		writeValue(it.second);
		firstElement_ = false;
	}

	outdent();
	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
	return *this;
}

template<typename Type>
std::enable_if_t<is_optional<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	if (value)
		writeValue<typename Type::value_type>(*value);
	else
		writeValue<typename Type::value_type>({});
	return *this;
}

template<typename Type>
inline std::enable_if_t<is_parameter_set<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}

	jsonString_ << "{" << std::endl;

	firstElement_ = true;
	tabCounter_++;
	const_cast<Type&>(value).configure(*this);
	firstElement_ = false;
	tabCounter_--;

	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
	return *this;
}

template<typename Type>
inline std::enable_if_t<
		!std::is_enum<Type>::value && !is_string<Type>::value && !is_angle<Type>::value &&
		!is_vector<Type>::value && !is_parameter_set<Type>::value && !is_string_key_map<Type>::value &&
		!is_optional<Type>::value && !is_eigen<Type>::value, JsonPopulator>&
JsonPopulator::writeValue(const Type& value)
{
	if constexpr (std::is_same_v<Type, bool>)
		jsonString_ << (value ? "true" : "false");
	else
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
	(populateImpl<Objects>(), ...);
}

template<template<class...Args> class Container, class ... Objects>
inline void
JsonPopulator::populateContainer(const Container<Objects...>&)
{
	(populateImpl<Objects>(), ...);
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
	jsonString_ << "\"" << Obj::typeId << "\"" << ": {" << std::endl;
//	addTabs();
	indent();
	firstElement_ = true;

	handleParams<Obj>();

	outdent();
	firstElement_ = false;

	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
}


template<class Obj, std::enable_if_t<is_static_factory<Obj>::value, int>>
inline void
JsonPopulator::populateImpl()
{
	//Is configurable Object
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	addTabs();
	jsonString_ << "\"" << Obj::typeId << "\"" << ": {" << std::endl;
	indent();
	firstElement_ = true;
	populateContainer(typename Obj::SpecificObjects());
	outdent();
	firstElement_ = false;

	jsonString_ << std::endl;
	addTabs();
	jsonString_ << "}";
}


template<class Obj, std::enable_if_t<is_static_helper<Obj>::value, int>>
inline void
JsonPopulator::populateImpl()
{
	//Is configurable Object
	if (!firstElement_)
	{
		jsonString_ << "," << std::endl;
	}
	populateContainer(Obj());
}


template<class Obj, std::enable_if_t<
		!is_configurable_object<Obj>::value && !is_static_factory<Obj>::value && !is_static_helper<Obj>::value, int>>
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
	jsonString_ << "\"" << Obj::typeId << "\"" << ": {" << std::endl;
	addTabs();
	jsonString_ << "}";
}


template<class Obj>
inline bool
JsonPopulator::populateImplBoolRet()
{
	populateImpl<Obj>();
	return true;
}


template<class Container>
int
JsonPopulator::fromMain(int argc, char** argv)
{
	std::string filename = "a.json";
	if (argc >= 2)
	{
		filename = argv[1];
	}

	std::ofstream file;

	file.open(filename, std::ofstream::out);

	JsonPopulator pop(file);

	pop.populateContainer(Container());

	return 0;
}


template<class Obj, std::enable_if_t<
		has_configure_params<Obj>::value, int>>
void
JsonPopulator::handleParams()
{
	Obj obj;
	obj.configureParams(*this);
}


template<class Obj, std::enable_if_t<
		!has_configure_params<Obj>::value, int>>
void
JsonPopulator::handleParams()
{
	Obj::configureParamsStatic(*this);
}


#endif /* UAVAP_CORE_PROPERTYMAPPER_JSONPOPULATOR_H_ */
