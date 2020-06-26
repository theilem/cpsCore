/*
 * PropertyMapperTest.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: mircot
 */

#include <cpsCore/Utilities/Test/TestInfo.h>
#include <boost/property_tree/json_parser.hpp>
#include <cpsCore/Configuration/Configuration.hpp>
#include <cpsCore/Configuration/PropertyMapper.hpp>


TEST_CASE("Vector of Doubles")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	Configuration config;
	boost::property_tree::read_json(test_info::test_dir() + "Utilities/config/pm_test.json",
									config);

	PropertyMapper<Configuration> pm(config);
	std::vector<double> vec;
	REQUIRE(pm.addVector<std::vector<double>>("vec_double", vec, true));

	REQUIRE(vec.size() == 4);
	CHECK(vec[0] == 1.0);
	CHECK(vec[1] == 2.0);
	CHECK(vec[2] == 3.0);
	CHECK(vec[3] == 4.0);

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}

TEST_CASE("Vector Test 2")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	Configuration config;
	boost::property_tree::read_json(test_info::test_dir() + "Utilities/config/pm_test.json",
									config);

	PropertyMapper<Configuration> pm(config);
	std::vector<Configuration> edges;
	pm.addVector("edges", edges, true);

	std::vector<Vector2> vec;

	for (const auto& it : edges)
	{
		PropertyMapper<Configuration> edge(it);
		Vector2 e;
		if (edge.add("", e, true))
			vec.push_back(e);
	}

	REQUIRE(vec.size() == 4);
	CHECK(vec[0] == Vector2(1, 1));
	CHECK(vec[1] == Vector2(2, 1));
	CHECK(vec[2] == Vector2(1, 2));
	CHECK(vec[3] == Vector2(2, 2));

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}


TEST_CASE("Map test")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	Configuration config;
	boost::property_tree::read_json(test_info::test_dir() + "Utilities/config/pm_test.json",
									config);

	PropertyMapper<Configuration> pm(config);
	std::map<std::string, int> missions;
	pm.addMap<std::map<std::string, int>>("missions", missions, true);

	REQUIRE(missions.size() == 2);
	CHECK(missions.find("mission1")->second == 8);
	CHECK(missions.find("mission2")->second == 10);
	CHECK(missions.find("mission3") == missions.end());

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}


TEST_CASE("Unordered map test")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	Configuration config;
	boost::property_tree::read_json(test_info::test_dir() + "Utilities/config/pm_test.json",
									config);

	PropertyMapper<Configuration> pm(config);
	std::unordered_map<std::string, int> missions;
	pm.addMap<std::unordered_map<std::string, int>>("missions", missions, true);

	REQUIRE(missions.size() == 2);
	CHECK(missions.find("mission1")->second == 8);
	CHECK(missions.find("mission2")->second == 10);
	CHECK(missions.find("mission3") == missions.end());

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}


TEST_CASE("Optional test")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	Configuration config;
	boost::property_tree::read_json(test_info::test_dir() + "Utilities/config/pm_test.json",
									config);

	PropertyMapper<Configuration> pm(config);
	Optional<int> opt1;
	Optional<int> opt2;
	pm.addOptional<int>("optional1", opt1, true);
	pm.addOptional<int>("optional2", opt2, false);

	CHECK(!opt2.has_value());
	REQUIRE(opt1.has_value());
	CHECK(*opt1 == 7);

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}
