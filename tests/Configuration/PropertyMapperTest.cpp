/*
 * PropertyMapperTest.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: mircot
 */

#include <catch2/catch.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cpsCore/Configuration/Configuration.hpp>
#include <cpsCore/Configuration/PropertyMapper.hpp>


TEST_CASE("Vector of Doubles")
{
	Configuration config;
	boost::property_tree::read_json("Utilities/config/pm_test.json",
			config);

	PropertyMapper<Configuration> pm(config);
	std::vector<double> vec;
	REQUIRE(pm.addVector<std::vector<double>>("vec_double", vec, true));

	REQUIRE(vec.size() == 4);
	CHECK(vec[0] == 1.0);
	CHECK(vec[1] == 2.0);
	CHECK(vec[2] == 3.0);
	CHECK(vec[3] == 4.0);

}

TEST_CASE("Vector Test 2")
{
	Configuration config;
	boost::property_tree::read_json("Utilities/config/pm_test.json",
			config);

	PropertyMapper<Configuration> pm(config);
	std::vector<Configuration> edges;
	pm.addVector("edges", edges, true);

	std::vector<Vector2> vec;

	for (const auto& it : edges)
	{
		PropertyMapper<Configuration> edge(it);
		Vector2 e;
		if (edge.add("",e,true))
			vec.push_back(e);
	}

	REQUIRE(vec.size() == 4);
	CHECK(vec[0] == Vector2(1,1));
	CHECK(vec[1] == Vector2(2,1));
	CHECK(vec[2] == Vector2(1,2));
	CHECK(vec[3] == Vector2(2,2));

}
