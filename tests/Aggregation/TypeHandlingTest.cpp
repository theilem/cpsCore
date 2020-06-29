//
// Created by mirco on 23.11.19.
//


#include <cpsCore/Aggregation/AggregatableObject.hpp>
#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Framework/StaticHelper.h>

namespace
{

class TestB;

class TestA : public AggregatableObject<TestB>
{
public:
	int testVal;

	static constexpr TypeId typeId = "test_a";
};

class TestC : public AggregatableObject<>
{
public:
	int testVal;
	// Missing typeid
};

}

TEST_CASE("Incomplete Type handling")
{
	auto scope = CPSLogger::LogLevelScope(LogLevel::NONE);

	using ThisTestCasesDefaults = StaticHelper<TestA>;

	//This should compile even because it doesn't care about TestB and thus should ignore that it is of incomplete Type
	auto agg = StaticHelper<ThisTestCasesDefaults>::createAggregation(Configuration());

/*
	using ThisTestCasesDefaults = StaticHelper<TestA, TestB>;
//	This should not compile, since TestB is incomplete but would need to be created
	auto agg = StaticHelper<ThisTestCasesDefaults>::createAggregation(Configuration());
 */

/*
	using ThisTestCasesDefaults = StaticHelper<TestA>;
//	This should not compile either, since TestB is incomplete but would need to be created potentially
	auto agg = StaticHelper<ThisTestCasesDefaults, TestB>::createAggregation(Configuration());
*/

	CHECK(agg.getOne<TestA>());
	CHECK_FALSE(agg.getOne<TestB>()); // TestB is incomplete Type so it should return a nullptr.
	//This assumes that if a class is incomplete it can be ignored
}

TEST_CASE("TypeId handling")
{
	auto scope = CPSLogger::LogLevelScope(LogLevel::NONE);

	CHECK(has_typeid<TestA>::value);
	CHECK_FALSE(has_typeid<TestC>::value);
}
