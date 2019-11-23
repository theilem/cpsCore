//
// Created by mirco on 23.11.19.
//


#include <cpsCore/Aggregation/AggregatableObject.hpp>
#include <cpsCore/Aggregation/Aggregator.h>
#include "TestInfo.h"

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

TEST_CASE("Checking objects can be Aggregated")
{
	auto testa = std::make_shared<TestA>();
	auto testb = std::make_shared<TestB>();
	auto testc = std::make_shared<TestC>();

	testa->testVal = 1;

	REQUIRE_FALSE(testa->isSet<TestB>());

	auto agg = Aggregator::aggregate({testa, testb, testc});

	CHECK(!testa->isSet<TestA>());
	CHECK(testa->isSet<TestB>());
	CHECK(testa->isSet<TestC>());
	CHECK(testa->checkIsSet<TestB, TestC>());

	CHECK(!testb->isSet<TestB>());
	CHECK(testb->isSet<TestA>());
	CHECK(testb->isSet<TestC>());
	CHECK(testb->checkIsSet<TestA, TestC>());

	CHECK(!testc->isSet<TestC>());
	CHECK(testc->isSet<TestA>());
	CHECK(testc->isSet<TestB>());
	CHECK(testc->checkIsSet<TestA, TestB>());

	CHECK(testb->get<TestA>()->testVal == 1);
	CHECK(testc->get<TestA>()->testVal == 1);

	testb->get<TestA>()->testVal = 2;

	CHECK(testa->testVal == 2);
}
