//
// Created by mirco on 15.12.19.
//

#ifndef CPSCORE_AGGREGATION_TYPETRAITS_HPP
#define CPSCORE_AGGREGATION_TYPETRAITS_HPP

#include <type_traits>


template <typename T>
struct is_complete_impl {
	template <typename U>
	static auto test(U*)  -> std::integral_constant<bool, sizeof(U) == sizeof(U)>;
	static auto test(...) -> std::false_type;
	using type = decltype(test((T*)0));
};

template <typename T>
struct is_complete : is_complete_impl<T>::type {};

template<class AggObj>
struct has_typeid_impl{
	template<class U>
	static auto test(U*) -> std::integral_constant<bool, sizeof(U::typeId) == sizeof(U::typeId)>;
	static auto test(...) -> std::false_type;
	using type = decltype(test((AggObj*)0));
};

template<class AggObj>
struct has_typeid : has_typeid_impl<AggObj>::type {};





#endif //CPSCORE_AGGREGATION_TYPETRAITS_HPP
