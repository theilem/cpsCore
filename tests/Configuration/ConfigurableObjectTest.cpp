/*
 * ConfigurableObjectTest.cpp
 *
 *  Created on: Jun 24, 2019
 *      Author: mirco
 */
#include <cpsCore/Configuration/ConfigurableObject.hpp>
#include "TestInfo.h"

namespace
{
struct ParamsNested
{
	Parameter<float> p1 = { 2.5, "p1", true };
	Parameter<std::string> p2 = { "test", "p2", false };
	Parameter<uint64_t> p3 = { 100, "p3", true };

	template<class Configurator>
	inline void
	configure(Configurator& c)
	{
		c & p1;
		c & p2;
		c & p3;
	}
};

struct Params
{
	Parameter<float> p1 = { 1.0, "p1", true };
	Parameter<int> p2 = { 7, "p2", true };
	Parameter<ParamsNested> p3 = { { }, "p3", false };

	template<class Configurator>
	inline void
	configure(Configurator& c)
	{
		c & p1;
		c & p2;
		c & p3;
	}
};

class Test: public ConfigurableObject<Params>
{
public:
	void
	checkParams()
	{
		CHECK(params.p1() == Approx(2.1).margin(1e-4));
		CHECK(params.p2() == 1);
		CHECK(params.p3().p1() == Approx(3.2).margin(1e-4));
		CHECK(params.p3().p2() == "test1");
		CHECK(params.p3().p3() == 700);
	}
};

class Test2: public ConfigurableObject<ParamsNested>
{
public:
	static constexpr const char* const typeId = "test2";

	bool
	configure(const Configuration& config)
	{
		PropertyMapper<Configuration> pm(config);

		configureParams(pm);

		return pm.map();
	}

	template<typename Config>
	void
	configureParams(Config& c)
	{
		params.configure(c);

		ParameterRef<Test> ref(test, {}, "sub_test", true);
		c & ref;

	}

	void
	checkParams()
	{
		CHECK(params.p1() == Approx(4.2).margin(1e-4));
		CHECK(params.p2() == "test2");
		CHECK(params.p3() == 800);

		test.checkParams();
	}

private:

	Test test;

};

}

TEST_CASE("Test1")
{

	Configuration config;
	Configuration subConfig;

	subConfig.add("p1", 3.2);
	subConfig.add("p2", "test1");
	subConfig.add("p3", 700);

	config.add("p1", 2.1);
	config.add("p2", 1);
	config.add_child("p3", subConfig);

	Test test;
	CHECK(test.configure(config));
	test.checkParams();

	CHECK_FALSE(is_parameter_set<float>::value);
	CHECK(is_parameter_set<Params>::value);
	CHECK(is_parameter_set<ParamsNested>::value);

}

TEST_CASE("Test2 Member Config")
{

	Configuration config;
	Configuration subConfig;

	subConfig.add("p1", 3.2);
	subConfig.add("p2", "test1");
	subConfig.add("p3", 700);

	config.add("p1", 2.1);
	config.add("p2", 1);
	config.add_child("p3", subConfig);

	Configuration configUpper;

	configUpper.add("p1", 4.2);
	configUpper.add("p2", "test2");
	configUpper.add("p3", 800);
	configUpper.add_child("sub_test", config);

	Test2 test;
	CHECK(test.configure(configUpper));
	test.checkParams();



}
