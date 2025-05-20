/*
 * ConfigurableObjectTest.cpp
 *
 *  Created on: Jun 24, 2019
 *      Author: mirco
 */
#include <cpsCore/Configuration/ConfigurableObject.hpp>
#include <cpsCore/Utilities/Test/TestInfo.h>

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

struct ParamsWithOptional
{
	Parameter<Optional<ParamsNested>> nested = { {}, "nested", false };

	template<class Configurator>
	inline void
	configure(Configurator& c)
	{
		c & nested;
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

class TestOptional: public ConfigurableObject<ParamsWithOptional>
{
public:
};

class Test2: public ConfigurableObject<ParamsNested>
{
public:
	static constexpr auto typeId = "test2";

	bool
	configure(const Configuration& config)
	{
		PropertyMapper pm(config);

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
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);

	Configuration config;
	Configuration subConfig;

	subConfig["p1"] = 3.2;
	subConfig["p2"] = "test1";
	subConfig["p3"] = 700;

	config["p1"] = 2.1;
	config["p2"] = 1;
	config["p3"] = subConfig;

	Test test;
	CHECK(test.configure(config));
	test.checkParams();

	CHECK_FALSE(is_parameter_set<float>::value);
	CHECK(is_parameter_set<Params>::value);
	CHECK(is_parameter_set<ParamsNested>::value);

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);
}

TEST_CASE("Test2 Member Config")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);

	Configuration config;
	Configuration subConfig;

	subConfig["p1"] = 3.2;
	subConfig["p2"] = "test1";
	subConfig["p3"] = 700;

	config["p1"] = 2.1;
	config["p2"] = 1;
	config["p3"] = subConfig;

	Configuration configUpper;

	configUpper["p1"] = 4.2;
	configUpper["p2"] = "test2";
	configUpper["p3"] = 800;
	configUpper["sub_test"] = config;

	Test2 test;
	CHECK(test.configure(configUpper));
	test.checkParams();

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);


}

TEST_CASE("Test3 Optional Config")
{
	CPSLogger::instance()->setLogLevel(LogLevel::NONE);

	Configuration config;
	Configuration configWithout;
	Configuration subConfig;


	subConfig["p1"] = 3.2;
	subConfig["p2"] = "test1";
	subConfig["p3"] = 700;

	config["nested"] = subConfig;

	TestOptional test;
	CHECK(test.configure(config));
	CHECK(test.getParams().nested());
	CHECK(test.getParams().nested()->p1() == 3.2f);

	Configuration c;
	PropertyMapper p(c);
	REQUIRE(p.isEmpty());

	TestOptional test2;
	bool conf = test2.configure(configWithout);
	CHECK(conf);
	CHECK(!test2.getParams().nested());

	CPSLogger::instance()->setLogLevel(LogLevel::DEBUG);


}
