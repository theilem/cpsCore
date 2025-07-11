//
// Created by mirco on 15.12.19.
//

#ifndef CPSCORE_CONFIGURATION_TYPETRAITS_HPP
#define CPSCORE_CONFIGURATION_TYPETRAITS_HPP

#include <type_traits>
#include "cpsCore/Configuration/Configuration.hpp"

class PropertyMapper;

// template <typename T, typename = void>
// struct is_configurable_object : std::false_type
// {
// };
//
// template <typename T>
// struct is_configurable_object<T, std::void_t<
//                                   decltype(std::declval<typename T::ParamType>().configure(
//                                       std::declval<PropertyMapper&>()))
//                               >> : std::true_type
// {
// };

template <typename T>
using is_configurable_object = std::is_base_of<IConfigurableObject, T>;

template <typename, typename = void>
struct is_parameter_set_ref : std::false_type
{
};

template <typename T>
struct is_parameter_set_ref<T, std::void_t<
                                decltype(configure(std::declval<PropertyMapper&>(), std::declval<T&>()))
                            >> : std::true_type
{
};

template <typename, typename = void>
struct has_configure_params : std::false_type
{
};

template <typename T>
struct has_configure_params<T, std::void_t<
                                decltype(std::declval<T>().configureParams(std::declval<PropertyMapper&>()))
                            >> : std::true_type
{
};


#endif //CPSCORE_CONFIGURATION_TYPETRAITS_HPP
