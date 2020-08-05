//
// Created by mirco on 05.08.20.
//

#ifndef CPSCORE_TERMINALCONFIGURATOR_H
#define CPSCORE_TERMINALCONFIGURATOR_H

#include <iostream>
#include <sstream>
#include "cpsCore/Configuration/Parameter.hpp"
#include "cpsCore/Utilities/EnumMap.hpp"

class TerminalConfigurator
{
public:

	template<typename ConfObj>
	bool
	configure(ConfObj& obj);

	template<typename Param>
	TerminalConfigurator&
	operator&(Param&);

private:

	template<typename Param>
	void
	handleParameter(Param& param);

	template<typename ParamSet>
	void
	handleParameterSet(ParamSet& param);

	template<typename ValueType>
	void
	setValue(ValueType& val, const std::string& str);

	template<typename ValueType>
	std::string
	toString(ValueType& val);
};

template<typename ConfObj>
inline bool
TerminalConfigurator::configure(ConfObj& obj)
{
	static_assert(is_parameter_set<typename ConfObj::ParamType>::value, "Can only configure ConfigurableObjects");
}

template<typename Param>
TerminalConfigurator&
TerminalConfigurator::operator&(Param& param)
{
	if constexpr (is_parameter<Param>::value)
	{
		handleParameter(param);
	}
	else if constexpr (is_parameter_set<Param>::value)
	{
		handleParameterSet(param);
	}
	else
	{
		static_assert(is_parameter<Param>::value, "Ha");
	}
}

template<typename Param>
void
TerminalConfigurator::handleParameter(Param& param)
{
	std::cout << param.id;
	std::cout << " [" << toString(param.value) << "]: ";
	std::string response;
	std::getline(std::cin, response);
	if (!response.empty())
	{
		setValue(param.value, response);
	}
}

template<typename ParamSet>
void
TerminalConfigurator::handleParameterSet(ParamSet& param)
{
	param.configure(*this);
}

template<typename ValueType>
void
TerminalConfigurator::setValue(ValueType& val, const std::string& str)
{
	if constexpr (std::is_enum<ValueType>::value)
	{
		val = EnumMap<ValueType>::convert(str);
	}
	else
	{
		std::stringstream ss;
		ss.rdbuf()->str(str);
		ss >> val;
	}
}

template<typename ValueType>
std::string
TerminalConfigurator::toString(ValueType& val)
{
	if constexpr (std::is_enum<ValueType>::value)
	{
		return EnumMap<ValueType>::convert(val);
	}
	else
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
}

#endif //CPSCORE_TERMINALCONFIGURATOR_H
