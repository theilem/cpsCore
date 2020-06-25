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

TEST_CASE("TypeTraits")
{

	CHECK(is_string_key_map<std::map<std::string, std::string>>::value);
	CHECK(is_string_key_map<std::map<std::string, int>>::value);
	CHECK(!is_string_key_map<std::map<int, int>>::value);

	CHECK(is_string_key_map<std::unordered_map<std::string, std::string>>::value);
	CHECK(is_string_key_map<std::unordered_map<std::string, int>>::value);
	CHECK(!is_string_key_map<std::unordered_map<int, int>>::value);

	CHECK(!is_string_key_map<std::vector<int>>::value);
}
