//
// Created by mirco on 23.11.19.
//


#include <cpsCore/Aggregation/AggregatableObject.hpp>
#include <cpsCore/Utilities/Test/TestInfo.h>

namespace
{

class TestA;

class TestB;

class TestC;

class TestA : public AggregatableObject<TestB, TestC>
{
public:
	int testVal;

	static constexpr TypeId typeId = "test_a";
};

class TestB : public AggregatableObject<TestA, TestC>
{
public:
	int testVal;

	static constexpr TypeId typeId = "test_b";

};

class TestC : public AggregatableObject<TestA, TestB>
{
public:
	int testVal;

	static constexpr TypeId typeId = "test_c";

};

}

SCENARIO("Checking objects can be Aggregated")
{
	CPSLogger::LogLevelScope scope(LogLevel::NONE);
	GIVEN("Items to be aggregated")
	{
		auto testa = std::make_shared<TestA>();
		auto testb = std::make_shared<TestB>();
		auto testc = std::make_shared<TestC>();

		testa->testVal = 1;

		REQUIRE_FALSE(testa->isSet<TestB>());

		WHEN("Aggregation is invoked")
		{
			auto agg = Aggregator::aggregate({testa, testb, testc});

			THEN("TestA Correctly aggregated")
			{
//				CHECK(!testa->isSet<TestA>()); This will static assert, as TestA is not aggregated to itself
				CHECK(testa->isSet<TestB>());
				CHECK(testa->isSet<TestC>());
//				CHECK(!testa->isSet<TestA, TestB>());  Static assert
//				CHECK(!testa->isSet<TestA, TestC>());  Static assert
				CHECK(testa->isSet<TestB, TestC>());
//				CHECK(!testa->isSet<TestA, TestB, TestC>()); Static assert

//				CHECK(!testa->checkIsSet<TestA>()); Static assert
				CHECK(testa->checkIsSet<TestB>());
				CHECK(testa->checkIsSet<TestC>());
//				CHECK(!testa->checkIsSet<TestA, TestB>()); Static assert
//				CHECK(!testa->checkIsSet<TestA, TestC>()); Static assert
				CHECK(testa->checkIsSet<TestB, TestC>());
//				CHECK(!testa->checkIsSet<TestA, TestB, TestC>()); Static assert
				CHECK(testa->checkIsSetAll());
			}

			THEN("TestB Correctly aggregated")
			{
				CHECK(testb->isSet<TestA>());
//				CHECK(!testb->isSet<TestB>()); Static assert
				CHECK(testb->isSet<TestC>());
//				CHECK(!testb->isSet<TestA, TestB>()); Static assert
				CHECK(testb->isSet<TestA, TestC>());
//				CHECK(!testb->isSet<TestB, TestC>()); Static assert
//				CHECK(!testb->isSet<TestA, TestB, TestC>()); Static assert
				CHECK(testb->checkIsSetAll());

				CHECK(testb->checkIsSet<TestA>());
//				CHECK(!testb->checkIsSet<TestB>()); This will static assert, as TestB is not aggregated to itself
				CHECK(testb->checkIsSet<TestC>());
//				CHECK(!testb->checkIsSet<TestA, TestB>()); Static assert
				CHECK(testb->checkIsSet<TestA, TestC>());
//				CHECK(!testb->checkIsSet<TestB, TestC>()); Static assert
			}

			THEN("TestC Correctly aggregated")
			{
				CHECK(testc->isSet<TestA>());
				CHECK(testc->isSet<TestB>());
//				CHECK(!testc->isSet<TestC>()); Static assert
				CHECK(testc->isSet<TestA, TestB>());
//				CHECK(!testc->isSet<TestA, TestC>()); Static assert
//				CHECK(!testc->isSet<TestB, TestC>()); Static assert
//				CHECK(!testc->isSet<TestA, TestB, TestC>()); Static assert
				CHECK(testc->checkIsSetAll());

				CHECK(testc->checkIsSet<TestA>());
				CHECK(testc->checkIsSet<TestB>());
//				CHECK(!testc->checkIsSet<TestC>()); Static assert
				CHECK(testc->checkIsSet<TestA, TestB>());
//				CHECK(!testc->checkIsSet<TestA, TestC>()); Static assert
//				CHECK(!testc->checkIsSet<TestB, TestC>()); Static assert
			}

			THEN("Values correctly set")
			{
				CHECK(testb->get<TestA>()->testVal == 1);
				CHECK(testc->get<TestA>()->testVal == 1);

				testb->get<TestA>()->testVal = 2;

				CHECK(testa->testVal == 2);
			}
		}
	}
}

SCENARIO("Checking missing object in agg")
{
	CPSLogger::LogLevelScope scope(LogLevel::NONE);
	GIVEN("Items to be aggregated")
	{
		auto testa = std::make_shared<TestA>();
		auto testb = std::make_shared<TestB>();

		testa->testVal = 1;

		REQUIRE_FALSE(testa->isSet<TestB>());

		WHEN("Aggregation is invoked")
		{
			auto agg = Aggregator::aggregate({testa, testb});

			THEN("TestA Correctly aggregated")
			{
//				CHECK(!testa->isSet<TestA>()); Static assert
				CHECK(testa->isSet<TestB>());
				CHECK(!testa->isSet<TestC>());
//				CHECK(!testa->isSet<TestA, TestB>()); Static assert
//				CHECK(!testa->isSet<TestA, TestC>()); Static assert
				CHECK(!testa->isSet<TestB, TestC>());
//				CHECK(!testa->isSet<TestA, TestB, TestC>()); Static assert

//				CHECK(!testa->checkIsSet<TestA>()); Static assert
				CHECK(testa->checkIsSet<TestB>());
				CHECK(!testa->checkIsSet<TestC>());
//				CHECK(!testa->checkIsSet<TestA, TestB>()); Static assert
//				CHECK(!testa->checkIsSet<TestA, TestC>()); Static assert
				CHECK(!testa->checkIsSet<TestB, TestC>());
//				CHECK(!testa->checkIsSet<TestA, TestB, TestC>()); Static assert
				CHECK(!testa->checkIsSetAll());
			}

			THEN("TestB Correctly aggregated")
			{
				CHECK(testb->isSet<TestA>());
//				CHECK(!testb->isSet<TestB>()); Static assert
				CHECK(!testb->isSet<TestC>());
//				CHECK(!testb->isSet<TestA, TestB>()); Static assert
				CHECK(!testb->isSet<TestA, TestC>());
//				CHECK(!testb->isSet<TestB, TestC>()); Static assert
//				CHECK(!testb->isSet<TestA, TestB, TestC>()); Static assert
				CHECK(!testb->checkIsSetAll());

				CHECK(testb->checkIsSet<TestA>());
//				CHECK(!testb->checkIsSet<TestB>()); Static assert
				CHECK(!testb->checkIsSet<TestC>());
//				CHECK(!testb->checkIsSet<TestA, TestB>()); Static assert
				CHECK(!testb->checkIsSet<TestA, TestC>());
//				CHECK(!testb->checkIsSet<TestB, TestC>()); Static assert
			}

			THEN("Values correctly set")
			{
				CHECK(testb->get<TestA>()->testVal == 1);

				testb->get<TestA>()->testVal = 2;

				CHECK(testa->testVal == 2);
			}
		}
	}
}
