//
// Created by mirco on 23.11.19.
//


#include <cpsCore/Aggregation/AggregatableObject.hpp>

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


BOOST_AUTO_TEST_SUITE(ObjectHandleTests)

BOOST_AUTO_TEST_CASE(Aggregation)
		{
				auto testa = std::make_shared<TestA>();
		auto testb = std::make_shared<TestB>();
		auto testc = std::make_shared<TestC>();

		testa->testVal = 1;

		BOOST_CHECK(!testa->isSet<TestB>());

		auto agg = Aggregator::aggregate({ testa, testb, testc });

		BOOST_REQUIRE(testa->isSet<TestB>());

		BOOST_CHECK_EQUAL(testb->get<TestA>()->testVal, 1);
		BOOST_CHECK_EQUAL(testc->get<TestA>()->testVal, 1);

		testb->get<TestA>()->testVal = 2;

		BOOST_CHECK_EQUAL(testa->testVal, 2);


		}

BOOST_AUTO_TEST_SUITE_END()



