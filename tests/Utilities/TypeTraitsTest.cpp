/*
 * PropertyMapperTest.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: mircot
 */

#include <cpsCore/Utilities/Test/TestInfo.h>
#include "cpsCore/Utilities/TypeTraits.hpp"
#include <map>
#include <unordered_map>

TEST_CASE("TypeTraits test")
{

	CHECK(is_string_key_map<std::map<std::string, std::string>>::value);
	CHECK(is_string_key_map<std::map<std::string, int>>::value);
	CHECK_FALSE(is_string_key_map<std::map<int, int>>::value);

	CHECK(is_string_key_map<std::unordered_map<std::string, std::string>>::value);
	CHECK(is_string_key_map<std::unordered_map<std::string, int>>::value);
	CHECK_FALSE(is_string_key_map<std::unordered_map<int, int>>::value);

	CHECK_FALSE(is_string_key_map<std::vector<int>>::value);

	CHECK(is_string<std::string>::value);
	CHECK(is_string<std::basic_string<char>>::value);
	CHECK(is_string<std::__cxx11::basic_string<char>>::value);

	CHECK(is_eigen<Eigen::Matrix<int, 4, 2>>::value);
	CHECK(is_eigen<Eigen::Matrix<int, 4, 2, Eigen::DontAlign>>::value);
	CHECK(is_eigen<Eigen::Matrix<double, 4, 2>>::value);
	CHECK(is_eigen<Eigen::Matrix<double, 4, 2, Eigen::DontAlign>>::value);
}
