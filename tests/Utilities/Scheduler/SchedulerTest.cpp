/*
 * SchedulerTest.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include <catch2/catch.hpp>
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Synchronization/AggregatableRunner.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/TimeProvider/SystemTimeProvider.h"

#include "cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h"

void
schedulingCount(int& counter)
{
	++counter;
}

TEST_CASE("MultiThreaded Test 1")
{
	auto tp = std::make_shared<MicroSimulator>();
	auto sched = std::make_shared<MultiThreadingScheduler>();
	auto runner = std::make_shared<AggregatableRunner>();
	Aggregator::aggregate(
			{tp, sched, runner});

	CPSLogger::instance()->setLogLevel(LogLevel::ERROR);

	int count1 = 0;

	sched->schedule(std::bind(schedulingCount, boost::ref(count1)), Milliseconds(0));

	runner->runAllStages();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK(count1 == 1);

	sched->stop();
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}

TEST_CASE("MultiThreaded Test 2")
{
	auto tp = std::make_shared<MicroSimulator>();
	auto sched = std::make_shared<MultiThreadingScheduler>();
	auto runner = std::make_shared<AggregatableRunner>();
	Aggregator::aggregate(
			{tp, sched, runner});
	CPSLogger::instance()->setLogLevel(LogLevel::ERROR);

	int count1 = 0;

	tp->stopOnWait();
	auto start = tp->now();

	sched->schedule(boost::bind(schedulingCount, boost::ref(count1)), Milliseconds(0),
					Milliseconds(10));

	runner->runAllStages();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(tp->now() - start == Milliseconds(0));
	CHECK(count1 == 1);

	tp->releaseWait();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(tp->now() - start == Milliseconds(10));
	CHECK(count1 == 2);

	tp->releaseWait();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(tp->now() - start == Milliseconds(20));
	CHECK(count1 == 3);

	sched->stop();
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}

TEST_CASE("MultiThreaded Test 3")
{
	auto tp = std::make_shared<MicroSimulator>();
	auto sched = std::make_shared<MultiThreadingScheduler>();
	auto runner = std::make_shared<AggregatableRunner>();
	Aggregator::aggregate(
			{tp, sched, runner});
	CPSLogger::instance()->setLogLevel(LogLevel::ERROR);

	int count1 = 0;
	int count2 = 0;

	auto start = tp->now();

	tp->stopOnWait();

	sched->schedule(std::bind(schedulingCount, boost::ref(count1)), Milliseconds(5),
					Milliseconds(10));

	runner->runAllStages();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(count1 == 0);

	tp->releaseWait();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(tp->now() - start == Milliseconds(5));
	CHECK(count1 == 1);

	sched->schedule(std::bind(schedulingCount, boost::ref(count2)), Milliseconds(0),
					Milliseconds(20));
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(count2 == 1);
	tp->releaseWait();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(tp->now() - start == Milliseconds(15));

	CHECK(count1 == 2);
	CHECK(count2 == 1);
	sched->stop();
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}

TEST_CASE("MultiThreaded Test 4")
{
	auto tp = std::make_shared<MicroSimulator>();
	auto sched = std::make_shared<MultiThreadingScheduler>();
	auto runner = std::make_shared<AggregatableRunner>();
	Aggregator::aggregate(
			{tp, sched, runner});
	CPSLogger::instance()->setLogLevel(LogLevel::ERROR);

	int count1 = 0;

	auto start = tp->now();

	tp->stopOnWait();

	auto event = sched->schedule(std::bind(schedulingCount, std::ref(count1)), Milliseconds(5),
								 Milliseconds(10));

	CHECK(!runner->runAllStages());

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK_FALSE(count1);

	tp->releaseWait();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	CHECK(tp->now() - start == Milliseconds(5));
	CHECK(count1 == 1);

	event.cancel();
	CHECK(event.isCanceled());
	tp->releaseWait();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK(tp->now() - start == Milliseconds(15));
	CHECK(count1 == 1);

	tp->releaseWait();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	CHECK(tp->now() - start == Milliseconds(15));
	CHECK(count1 == 1);

	sched->stop();
	CPSLogger::instance()->setLogLevel(LogLevel::WARN);
}