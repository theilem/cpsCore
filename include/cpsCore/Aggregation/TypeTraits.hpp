//
// Created by mirco on 15.12.19.
//

#ifndef CPSCORE_AGGREGATION_TYPETRAITS_HPP
#define CPSCORE_AGGREGATION_TYPETRAITS_HPP

#include <type_traits>


template <typename T>
struct is_complete_helper {
	template <typename U>
	static auto test(U*)  -> std::integral_constant<bool, sizeof(U) == sizeof(U)>;
	static auto test(...) -> std::false_type;
	using type = decltype(test((T*)0));
};

template <typename T>
struct is_complete : is_complete_helper<T>::type {};



#endif //CPSCORE_AGGREGATION_TYPETRAITS_HPP
