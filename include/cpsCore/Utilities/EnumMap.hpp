/*
 * EnumMap.hpp
 *
 *  Created on: Jul 27, 2018
 *      Author: mircot
 */

#ifndef UAVAP_CORE_ENUMMAP_HPP_
#define UAVAP_CORE_ENUMMAP_HPP_

#include <initializer_list>
#include <map>
#include <unordered_map>
#include <string>

#include "cpsCore/Logging/CPSLogger.h"

template<typename ENUM>
class EnumMap
{
public:

	EnumMap(const EnumMap<ENUM>&) = delete;

	void
	operator=(const EnumMap<ENUM>&) = delete;

	static EnumMap&
	getInstance()
	{
		static EnumMap<ENUM> instance;
		return instance;
	}

	static void
	initialize(std::initializer_list<std::pair<ENUM, std::string>> l)
	{
		for (const auto& it : l)
		{
			getInstance().left_.insert(it);
			getInstance().right_.insert(std::make_pair(it.second, it.first));
		}
	}

	static ENUM
	convert(const std::string& str)
	{
		auto it = getInstance().right_.find(str);
		if (it == getInstance().right_.end())
		{
			CPSLOG_ERROR << "Unknown enum name for " << str;
			return static_cast<ENUM>(0);
		}
		return it->second;
	}

	static std::string
	convert(ENUM e)
	{
		auto it = getInstance().left_.find(e);
		if (it == getInstance().left_.end())
		{
			CPSLOG_ERROR << "Undefined enum value " << static_cast<int>(e);
			return std::string();
		}
		return it->second;
	}

	auto
	begin() const
	{
		return left_.begin();
	}

	auto
	end() const
	{
		return left_.end();
	}

protected:


	std::map<ENUM, std::string> left_;
	std::unordered_map<std::string, ENUM> right_;

private:

	EnumMap() = default;


};

template<typename ENUM>
class EnumInitializer
{
public:

	EnumInitializer(std::initializer_list<std::pair<ENUM, std::string>> l)
	{
		EnumMap<ENUM>::initialize(l);
	}
};

template<typename First, typename Second>
std::pair<const First, Second>*
findInMap(std::map<First, Second>& map, const First& arg)
{
	typename std::map<First, Second>::iterator it = map.find(arg);
	if (it == map.end())
		return nullptr;
	return &(*it);
}

template<typename First, typename Second>
const std::pair<const First, Second>*
findInMap(const std::map<First, Second>& map, const First& arg)
{
	typename std::map<First, Second>::const_iterator it = map.find(arg);
	if (it == map.end())
		return nullptr;
	return &(*it);
}

#define ENUMMAP_INIT(e, ...)    const EnumInitializer<e> initializer_##e(__VA_ARGS__)

template<typename ENUM, typename RetType, class STRUCT>
RetType
enumAccess(const STRUCT& data, const std::string& string)
{
	static_assert(std::is_enum<ENUM>::value, "Has to be called on enums");
	return enumAccess<RetType>(data, EnumMap<ENUM>::convert(string));
}

template<typename RetType, typename ENUM>
RetType
enumAccessUnknown(const ENUM& e)
{
	CPSLOG_ERROR << "Requested value " << EnumMap<ENUM>::convert(e) << " not available";
	return 0;
}


template<class T, typename E>
struct TypeToEnum
{
};

template<typename E, E VALUE>
struct EnumToType
{
};

#define MATCH_TYPE_AND_ENUM(TYPE, ENUM)                    \
  template <>                                            \
  struct TypeToEnum<TYPE, decltype(ENUM)> {                    \
    static decltype(ENUM) v() { return ENUM; }            \
    static constexpr decltype(ENUM) value = ENUM;        \
  };                                                        \
  template <>                                            \
  struct EnumToType<decltype(ENUM), ENUM> {                \
    typedef TYPE Type;                                    \
  }

#endif /* UAVAP_CORE_ENUMMAP_HPP_ */
