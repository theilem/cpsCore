/*
 * PropertyMapperTest.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: mircot
 */

#include <cpsCore/Utilities/Test/TestInfo.h>
#include "cpsCore/Utilities/TypeTraits.hpp"
#include "cpsCore/Utilities/LinearAlgebra.h"
#include <map>
#include <unordered_map>
#include "cpsCore/Utilities/EnumMap.hpp"

namespace
{

enum class TestEnum
{
	TEST1,
	TEST2,
	TEST3
};

ENUMMAP_INIT(TestEnum,
			 {
				 { TestEnum::TEST1, "test1" },
				 { TestEnum::TEST2, "test2" },
				 { TestEnum::TEST3, "test3" }
			 });
}

TEST_CASE("TypeTraits test")
{

	CHECK(is_string_key_map<std::map<std::string, std::string>>::value);
	CHECK(is_string_key_map<std::map<std::string, int>>::value);
	CHECK_FALSE(is_string_key_map<std::map<int, int>>::value);

	CHECK(is_string_key_map<std::unordered_map<std::string, std::string>>::value);
	CHECK(is_string_key_map<std::unordered_map<std::string, int>>::value);
	CHECK_FALSE(is_string_key_map<std::unordered_map<int, int>>::value);

	CHECK_FALSE(is_string_key_map<std::vector<int>>::value);

	CHECK(std::is_enum_v<TestEnum>);
	CHECK_FALSE(std::is_enum_v<int>);

	CHECK(is_enum_map<std::unordered_map<TestEnum, std::string>>::value);
	CHECK(is_enum_map<std::map<TestEnum, int>>::value);
	CHECK_FALSE(is_enum_map<std::unordered_map<int, int>>::value);

	CHECK_FALSE(is_enum_map<std::vector<int>>::value);

	CHECK(is_string<std::string>::value);
	CHECK(is_string<std::basic_string<char>>::value);
	CHECK(is_string<std::__cxx11::basic_string<char>>::value);

	CHECK(is_eigen<Eigen::Matrix<int, 4, 2>>::value);
	CHECK(is_eigen<Eigen::Matrix<int, 4, 2, Eigen::DontAlign>>::value);
	CHECK(is_eigen<Eigen::Matrix<double, 4, 2>>::value);
	CHECK(is_eigen<Eigen::Matrix<double, 4, 2, Eigen::DontAlign>>::value);

	CHECK(is_eigen_vector<Vector3>::value);
	CHECK(is_eigen_vector<Eigen::Vector3d>::value);
	CHECK(is_eigen_vector<Eigen::Vector3f>::value);
	CHECK(is_eigen_vector<Eigen::Vector3i>::value);
	CHECK_FALSE(is_eigen_vector<Eigen::Matrix2d>::value);
}
