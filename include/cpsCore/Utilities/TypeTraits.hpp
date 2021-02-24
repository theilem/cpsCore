/*
 * TypeTraits.hpp
 *
 *  Created on: Aug 18, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_TYPETRAITS_HPP_
#define UAVAP_CORE_TYPETRAITS_HPP_

#include <string>
#include <type_traits>
#include <vector>
#include <map>
#include <unordered_map>
#include <cpsCore/Utilities/Optional.hpp>
#include <cpsCore/Utilities/LinearAlgebra.h>

template<typename T>
struct is_vector : public std::false_type
{
};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type
{
};

template<typename T>
struct is_string_key_map : public std::false_type
{
};

template<typename T>
struct is_string_key_map<std::map<std::string, T>> : public std::true_type
{
};

template<typename T>
struct is_string_key_map<std::unordered_map<std::string, T>> : public std::true_type
{
};

template<typename T>
struct is_enum_map : public std::false_type
{
};

template<typename K, typename T>
struct is_enum_map<std::map<K, T>> : public std::is_enum<K>
{
};

template<typename K, typename T>
struct is_enum_map<std::unordered_map<K, T>> : public std::is_enum<K>
{
};

template<typename T>
struct is_optional : public std::false_type
{
};

template<typename T>
struct is_optional<Optional<T>> : public std::true_type
{
};

template<typename T>
struct is_eigen : std::false_type
{
};

template<typename T, int r, int c, int o>
struct is_eigen<Eigen::Matrix<T, r, c, o>> : std::true_type
{
};

template<typename Type>
using enable_if_is_vector = typename std::enable_if<is_vector<Type>::value, Type>::type;
template<typename Type>
using enable_if_not_is_vector = typename std::enable_if<!is_vector<Type>::value, Type>::type;

template<typename Type>
using enable_if_is_string_key_map = typename std::enable_if<is_string_key_map<Type>::value, Type>::type;
template<typename Type>
using enable_if_is_not_string_key_map = typename std::enable_if<!is_string_key_map<Type>::value, Type>::type;

template<typename Type>
using enable_if_is_optional = typename std::enable_if<is_optional<Type>::value, Type>::type;

template<typename Type>
using enable_if_is_not_optional = typename std::enable_if<!is_optional<Type>::value, Type>::type;




template<typename Type>
using is_string = std::is_same<Type, std::string>;


#endif /* UAVAP_CORE_TYPETRAITS_HPP_ */
