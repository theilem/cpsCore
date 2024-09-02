/*
 * SchedulerTest.cpp
 *
 *  Created on: Jul 20, 2017
 *      Author: mircot
 */

#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Synchronization/AggregatableRunner.h>
#include <cpsCore/Synchronization/SimpleRunner.h>
#include <cpsCore/Utilities/Scheduler/MicroSimulator.h>

#include <cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h>
#include <cpsCore/Utilities/SignalHandler/SignalHandler.h>
#include <cpsCore/Utilities/TimeProvider/SystemTimeProvider.h>

#include <iostream>

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
    auto sigHand = std::make_shared<SignalHandler>();
    Aggregator::aggregate(
        {tp, sched, runner, sigHand});

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
    auto sigHand = std::make_shared<SignalHandler>();
    Aggregator::aggregate(
        {tp, sched, runner, sigHand});
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
    auto sigHand = std::make_shared<SignalHandler>();
    auto runner = std::make_shared<AggregatableRunner>();
    Aggregator::aggregate(
        {tp, sched, runner, sigHand});
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
    auto sigHand = std::make_shared<SignalHandler>();
    auto runner = std::make_shared<AggregatableRunner>();
    Aggregator::aggregate(
        {tp, sched, runner, sigHand});
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

TEST_CASE("MicroSimulator cancel test")
{
    bool shouldCall = true;

    MicroSimulator sim;
    auto event = sim.schedule([&shouldCall]() { CHECK(shouldCall); }, Milliseconds(100), Milliseconds(100));

    CHECK(!event.isCanceled());
    sim.simulate(Seconds(100));
    CHECK(!event.isCanceled());
    event.cancel();
    shouldCall = false;
    sim.simulate(Seconds(1));
}

TEST_CASE("Scheduler Period Change")
{
    auto sched = std::make_shared<MultiThreadingScheduler>();
    auto tp = std::make_shared<SystemTimeProvider>();
    auto agg = Aggregator::aggregate({
        sched,
        std::make_shared<SignalHandler>(),
        tp
    });

    int counter = 0;

    auto start = tp->now();

    Event testEvent = sched->schedule([&counter]()
    {
        counter++;
    }, Milliseconds(0), Milliseconds(3));
    auto runner = SimpleRunner(agg);
    CHECK(!runner.runAllStages());
    std::condition_variable cv;
    std::mutex m;
    std::unique_lock lock(m);
    tp->waitUntil(start + Milliseconds(10), cv, lock);
    // Events at 0, 3, 6, 9
    int count1 = counter;
    testEvent.changePeriod(Milliseconds(10));
    tp->waitUntil(start + Milliseconds(45), cv, lock);
    // Events at 12, 22, 32, 42
    int count2 = counter;
    sched->stop();

    CHECK(count1 == 4);
    CHECK(count2 == 8);
}
