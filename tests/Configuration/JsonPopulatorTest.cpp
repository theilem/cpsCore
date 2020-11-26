/*
 * JsonPopulatorTest.cpp
 *
 *  Created on: Aug 27, 2019
 *      Author: mirco
 */

#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Configuration/ConfigurableObject.hpp>
#include <cpsCore/Configuration/JsonPopulator.h>
#include <fstream>
#include <cpsCore/Utilities/Scheduler/IScheduler.h>
#include <cpsCore/Utilities/Scheduler/MicroSimulator.h>
#include <cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h>

namespace
{
struct ParamsNested
{
	Parameter<float> p1 =
			{2.5, "p1", true};
	Parameter<std::string> p2 =
			{"test", "p2", false};
	Parameter<uint64_t> p3 =
			{100, "p3", true};

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
	Parameter<float> p1 =
			{1.0, "p1", true};
	Parameter<std::vector<int>> p2 =
			{{7,8,9}, "p2", true};
	Parameter<ParamsNested> p3 =
			{
					{}, "p3", false};

	template<class Configurator>
	inline void
	configure(Configurator& c)
	{
		c & p1;
		c & p2;
		c & p3;
	}
};

class Test : public ConfigurableObject<Params>
{
public:

	static constexpr const char* const typeId = "test";

};

class Test2 : public ConfigurableObject<Params>
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

		ParameterRef<Test> ref(test,
							   {}, "sub_test", true);
		c & ref;

	}

private:

	Test test;

};

}

TEST_CASE("Json Populator Test 1")
{
	JsonPopulator pop;
	pop.populate<Test, Test>();

	std::string correct = "{\n"
						  "\t\"test\": {\n"
						  "\t\t\"p1\": 1,\n"
						  "\t\t\"p2\": [\n"
						  "\t\t\t7,\n"
						  "\t\t\t8,\n"
						  "\t\t\t9\n"
						  "\t\t],\n"
						  "\t\t\"p3\": {\n"
						  "\t\t\t\"p1\": 2.5,\n"
						  "\t\t\t\"p2\": \"test\",\n"
						  "\t\t\t\"p3\": 100\n"
						  "\t\t}\n"
						  "\t},\n"
						  "\t\"test\": {\n"
						  "\t\t\"p1\": 1,\n"
						  "\t\t\"p2\": [\n"
						  "\t\t\t7,\n"
						  "\t\t\t8,\n"
						  "\t\t\t9\n"
						  "\t\t],\n"
						  "\t\t\"p3\": {\n"
						  "\t\t\t\"p1\": 2.5,\n"
						  "\t\t\t\"p2\": \"test\",\n"
						  "\t\t\t\"p3\": 100\n"
						  "\t\t}\n"
						  "\t}\n"
						  "}";

	CHECK(correct == pop.getString());
}

TEST_CASE("Json Populator Test 2")
{
	REQUIRE(has_configure_params<Test2>::value);
	JsonPopulator pop;
	pop.populate<Test2>();



	std::string correct = "{\n"
						  "\t\"test2\": {\n"
						  "\t\t\"p1\": 1,\n"
						  "\t\t\"p2\": [\n"
						  "\t\t\t7,\n"
						  "\t\t\t8,\n"
						  "\t\t\t9\n"
						  "\t\t],\n"
						  "\t\t\"p3\": {\n"
						  "\t\t\t\"p1\": 2.5,\n"
						  "\t\t\t\"p2\": \"test\",\n"
						  "\t\t\t\"p3\": 100\n"
						  "\t\t},\n"
						  "\t\t\"sub_test\": {\n"
						  "\t\t\t\"p1\": 1,\n"
						  "\t\t\t\"p2\": [\n"
						  "\t\t\t\t7,\n"
						  "\t\t\t\t8,\n"
						  "\t\t\t\t9\n"
						  "\t\t\t],\n"
						  "\t\t\t\"p3\": {\n"
						  "\t\t\t\t\"p1\": 2.5,\n"
						  "\t\t\t\t\"p2\": \"test\",\n"
						  "\t\t\t\t\"p3\": 100\n"
						  "\t\t\t}\n"
						  "\t\t}\n"
						  "\t}\n"
						  "}";

	CHECK(correct == pop.getString());

}

