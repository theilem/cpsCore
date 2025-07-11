//
// Created by mirco on 11.07.25.
//

#ifndef TYPETRAITS_HPP
#define TYPETRAITS_HPP

#include <type_traits>

template <class...>
class StaticHelper;

template <typename T>
struct is_static_helper : std::false_type
{
};

template <class... Objs>
struct is_static_helper<StaticHelper<Objs...>> : std::true_type
{
};

template<class T, bool S, class...C>
class StaticFactory;

template<class T>
struct is_static_factory : std::false_type
{
};

template<class T, bool S, class...C>
struct is_static_factory<StaticFactory<T, S, C...>> : std::true_type
{
};


#endif //TYPETRAITS_HPP
