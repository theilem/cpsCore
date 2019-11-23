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


};

class TestB : public AggregatableObject<TestA, TestC>
{
public:

	int testVal;

};

class TestC : public AggregatableObject<TestA, TestB>
{
public:

	int testVal;

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

	REQUIRE(testa->isSet<TestB>());

	REQUIRE(testb->get<TestA>()->testVal == 1);
	REQUIRE(testc->get<TestA>()->testVal == 1);

	testb->get<TestA>()->testVal = 2;

	REQUIRE(testa->testVal == 2);
}
