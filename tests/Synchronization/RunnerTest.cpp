/*
 * RunnerTest.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: mircot
 */

#include <cpsCore/Aggregation/AggregatableObject.hpp>
#include <cpsCore/Synchronization/SynchronizedRunner.h>
#include <cpsCore/Synchronization/SynchronizedRunnerMaster.h>
#include <thread>

#include <catch2/catch.hpp>

class RunnableTestClass : public IRunnableObject, public AggregatableObject<>
{
public:

	RunnableTestClass() :
			lastRunStage(RunStage::SYNCHRONIZE)
	{
	}

	bool
	run(RunStage stage)
	{
		lastRunStage = stage;
		return false;
	}

	void
	notifyAggregationOnUpdate(const Aggregator& agg)
	{
	}

	RunStage lastRunStage;
};

class RunnableTestTimeoutClass : public IRunnableObject, public IAggregatableObject
{
public:

	RunnableTestTimeoutClass() :
			lastRunStage(RunStage::SYNCHRONIZE)
	{
	}

	bool
	run(RunStage stage)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		lastRunStage = stage;
		return false;
	}

	void
	notifyAggregationOnUpdate(const Aggregator& agg)
	{
	}

	RunStage lastRunStage;
};

TEST_CASE("Synchronized Runner Test")
{
	auto runObj1 = std::make_shared<RunnableTestClass>();
	auto runObj2 = std::make_shared<RunnableTestClass>();
	Aggregator agg1;
	agg1.add(runObj1);
	agg1.add(runObj2);

	auto runObj3 = std::make_shared<RunnableTestClass>();
	auto runObj4 = std::make_shared<RunnableTestClass>();
	Aggregator agg2;
	agg2.add(runObj3);
	agg2.add(runObj4);

	SynchronizedRunnerMaster master(2);

	std::shared_ptr<SynchronizedRunner> runner1(new SynchronizedRunner);
	std::shared_ptr<SynchronizedRunner> runner2(new SynchronizedRunner);

	CHECK((int) runObj1->lastRunStage == (int) RunStage::SYNCHRONIZE);
	CHECK((int) runObj2->lastRunStage == (int) RunStage::SYNCHRONIZE);
	CHECK((int) runObj3->lastRunStage == (int) RunStage::SYNCHRONIZE);
	CHECK((int) runObj4->lastRunStage == (int) RunStage::SYNCHRONIZE);

	std::thread run1(std::bind(&SynchronizedRunner::runSynchronized, runner1, std::ref(agg1)));
	std::thread run2(std::bind(&SynchronizedRunner::runSynchronized, runner2, std::ref(agg2)));

	CHECK(!master.runStage(RunStage::INIT));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK((int) runObj1->lastRunStage == (int) RunStage::INIT);
	CHECK((int) runObj2->lastRunStage == (int) RunStage::INIT);
	CHECK((int) runObj3->lastRunStage == (int) RunStage::INIT);
	CHECK((int) runObj4->lastRunStage == (int) RunStage::INIT);

	CHECK(!master.runStage(RunStage::NORMAL));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK((int) runObj1->lastRunStage == (int) RunStage::NORMAL);
	CHECK((int) runObj2->lastRunStage == (int) RunStage::NORMAL);
	CHECK((int) runObj3->lastRunStage == (int) RunStage::NORMAL);
	CHECK((int) runObj4->lastRunStage == (int) RunStage::NORMAL);

	CHECK(!master.runStage(RunStage::FINAL));

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK((int) runObj1->lastRunStage == (int) RunStage::FINAL);
	CHECK((int) runObj2->lastRunStage == (int) RunStage::FINAL);
	CHECK((int) runObj3->lastRunStage == (int) RunStage::FINAL);
	CHECK((int) runObj4->lastRunStage == (int) RunStage::FINAL);

	run1.join();
	run2.join();
}

TEST_CASE("Synchronized Runner Timeout")
{
	auto runObj1 = std::make_shared<RunnableTestTimeoutClass>();
	auto runObj2 = std::make_shared<RunnableTestClass>();
	Aggregator agg1;
	agg1.add(runObj1);
	agg1.add(runObj2);

	auto runObj3 = std::make_shared<RunnableTestClass>();
	auto runObj4 = std::make_shared<RunnableTestClass>();
	Aggregator agg2;
	agg2.add(runObj3);
	agg2.add(runObj4);

	SynchronizedRunnerMaster master(2);

	auto runner1 = std::make_shared<SynchronizedRunner>();
	auto runner2 = std::make_shared<SynchronizedRunner>();

	std::thread run1(
			std::bind(&SynchronizedRunner::runSynchronized, runner1, std::ref(agg1)));
	std::thread run2(
			std::bind(&SynchronizedRunner::runSynchronized, runner2, std::ref(agg2)));

	CPSLogger::instance()->setLogLevel(LogLevel::NONE);
	//Should timeout due to runObj1
	CHECK(master.runStage(RunStage::INIT));
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);

	run1.join();
	run2.join();
}
