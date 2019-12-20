//
// Created by mirco on 15.12.19.
//

#ifndef CPSCORE_CONFIGURATION_TYPETRAITS_HPP
#define CPSCORE_CONFIGURATION_TYPETRAITS_HPP


template<typename Type>
struct is_configurable_object
{
	template<typename _1>
	static char&
	chk(
			typename std::enable_if<
					std::is_same<void,
							decltype(std::declval<typename _1::ParamType>().configure(std::declval<int&>()))>::value,
					int>::type);

	template<typename>
	static int&
	chk(...);

	static constexpr bool value = sizeof(chk<Type>(0)) == sizeof(char);
};



template<typename Type>
struct is_parameter_set_ref
{
	template<typename _1> static char &
	chk(
			typename std::enable_if<
					std::is_same<void, decltype(configure(std::declval<int&>(), std::declval<_1&>()))>::value, int>::type);
	template<typename > static int &
	chk(...);

	static constexpr bool value = sizeof(chk<Type>(0)) == sizeof(char);
};

template<typename Type>
struct has_configure_params
{
	template<typename _1>
	static char&
	chk(
			typename std::enable_if<
					std::is_same<void,
							decltype(std::declval<_1>().configureParams(std::declval<int&>()))>::value,
					int>::type);

	template<typename>
	static int&
	chk(...);

	static constexpr bool value = sizeof(chk<Type>(0)) == sizeof(char);
};




#endif //CPSCORE_CONFIGURATION_TYPETRAITS_HPP
