/*
 * PropertyMapperTest.cpp
 *
 *  Created on: Aug 12, 2018
 *      Author: mircot
 */

#include <fstream>
#include <cpsCore/Utilities/EnumMap.hpp>


#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Configuration/Configuration.hpp>
#include <cpsCore/Configuration/PropertyMapper.hpp>


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

TEST_CASE("Vector of Doubles")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
	std::vector<double> vec;
	REQUIRE(pm.addVector("vec_double", vec, true));

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
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
	std::vector<Vector2> edges;
	pm.addVector("edges", edges, true);

	REQUIRE(edges.size() == 4);
	CHECK(edges[0] == Vector2(1, 1));
	CHECK(edges[1] == Vector2(2, 1));
	CHECK(edges[2] == Vector2(1, 2));
	CHECK(edges[3] == Vector2(2, 2));

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}


TEST_CASE("Map test")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
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
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
	std::unordered_map<std::string, int> missions;
	pm.addMap<std::unordered_map<std::string, int>>("missions", missions, true);

	REQUIRE(missions.size() == 2);
	CHECK(missions.find("mission1")->second == 8);
	CHECK(missions.find("mission2")->second == 10);
	CHECK(missions.find("mission3") == missions.end());

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}


TEST_CASE("Enum Map test")
{
	auto lvl = CPSLogger::LogLevelScope(LogLevel::NONE);
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
	std::map<TestEnum, int> enums;
	pm.addEnumMap<std::map<TestEnum, int>>("enums", enums, true);

	CHECK(enums.size() == 2);
	CHECK(enums.find(TestEnum::TEST1)->second == 8);
	CHECK(enums.find(TestEnum::TEST2)->second == 10);
	CHECK(enums.find(TestEnum::TEST3) == enums.end());

}


TEST_CASE("Optional test")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
	Optional<int> opt1;
	Optional<int> opt2;
	pm.addOptional<int>("optional1", opt1, true);
	pm.addOptional<int>("optional2", opt2, false);

	CHECK(!opt2.has_value());
	REQUIRE(opt1.has_value());
	CHECK(*opt1 == 7);

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}

TEST_CASE("Eigen Matrix test")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	std::ifstream file(test_info::test_dir() + "Utilities/config/pm_test.json");
	REQUIRE(file.is_open());
	auto config = Configuration::parse(file);

	PropertyMapper pm(config);
	Eigen::Matrix<int, 4, 2> matAligned;
	Eigen::Matrix<int, 4, 2, Eigen::DontAlign> matUnaligned;
	Eigen::Matrix<double, 3, 3> matDAligned;
	Eigen::Matrix<double, 3, 3, Eigen::DontAlign> matDUnaligned;
	pm.addEigen("eigen_matrix", matAligned, true);
	pm.addEigen("eigen_matrix", matUnaligned, true);
	pm.addEigen("eigen_matrix_d", matDAligned, true);
	pm.addEigen("eigen_matrix_d", matDUnaligned, true);

	Eigen::Matrix<int, 4, 2> correct(std::vector<int>({0, 1, 2, 3, 4, 5, 6, 7}).data());
	Eigen::Matrix<double, 3, 3> correctD(std::vector<double>({0.1, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9}).data());
	CHECK(matAligned == correct);
	CHECK(matUnaligned == correct);
	CHECK(matDAligned == correctD);
	CHECK(matDUnaligned == correctD);

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}
