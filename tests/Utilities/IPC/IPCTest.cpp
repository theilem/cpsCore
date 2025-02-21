/*
 * IPCTest.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: mircot
 */


#include <cpsCore/Utilities/Scheduler/MicroSimulator.h>
#include <cpsCore/Synchronization/SimpleRunner.h>
#include <cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h>
#include <cpsCore/Utilities/TimeProvider/SystemTimeProvider.h>

#include <cpsCore/Utilities/Test/TestInfo.h>
#include <functional>

struct Test
{
    int val1;
    float val2;
    Vector3 val3;
} test;

namespace dp
{
    template <class Archive, typename Type>
    inline void
    serialize(Archive& ar, Test& t)
    {
        ar & t.val1;
        ar & t.val2;
        ar & t.val3;
    }
}

#include "cpsCore/Utilities/IPC/IPC.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"

std::vector<Test> testVec;

void
checkValue(const Test& t, int& counter)
{
    CHECK(t.val1 == test.val1);
    CHECK(t.val2 == test.val2);
    CHECK(t.val3 == test.val3);
    ++counter;
}

void
checkVector(const std::vector<Test>& t, int& counter)
{
    REQUIRE(t.size() == testVec.size());
    int k = 0;
    for (auto& it : testVec)
    {
        CHECK(t[k].val1 == it.val1);
        CHECK(t[k].val2 == it.val2);
        CHECK(t[k].val3 == it.val3);
        k++;
    }
    ++counter;
}

TEST_CASE("IPC Test 1")
{
    {
        auto ipc = std::make_shared<IPC>();
        auto tp = std::make_shared<SystemTimeProvider>();
        auto sched = std::make_shared<MultiThreadingScheduler>();
        auto signalHandler = std::make_shared<SignalHandler>();
        auto agg = Aggregator::aggregate({ipc, tp, sched, signalHandler});

        IPCOptions opt;
        opt.multiTarget = true; // setting to shared memory
        auto publisher = ipc->publish<Test>("ipc_test_1", opt);

        int counter1 = 0;
        int counter2 = 0;
        Subscription sub1 = ipc->subscribe<Test>("ipc_test_1",
                                                 std::bind(&checkValue, std::placeholders::_1, std::ref(counter1)));
        Subscription sub2 = ipc->subscribe<Test>("ipc_test_1",
                                                 std::bind(&checkValue, std::placeholders::_1, std::ref(counter2)));
        REQUIRE(sub1.connected());
        REQUIRE(sub2.connected());
        SimpleRunner run(agg);
        REQUIRE(!run.runAllStages());
        test.val1 = 1;
        test.val2 = 1.2;
        test.val3 = Vector3(1, 2, 3);

        int checkCounter1 = counter1;
        int checkCounter2 = counter2;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        int checkCounter1_2 = counter1;
        int checkCounter2_2 = counter2;
        publisher.publish(test);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        int checkCounter1_3 = counter1;
        int checkCounter2_3 = counter2;

        test.val1 = 2;
        test.val2 = 2.2;
        test.val3 = Vector3(2, 3, 4);

        publisher.publish(test);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        int checkCounter1_4 = counter1;
        int checkCounter2_4 = counter2;

        CHECK(checkCounter1 == 0);
        CHECK(checkCounter2 == 0);

        CHECK(checkCounter1_2 == 0);
        CHECK(checkCounter2_2 == 0);

        CHECK(checkCounter1_3 == 1);
        CHECK(checkCounter2_3 == 1);

        CHECK(checkCounter1_4 == 2);
        CHECK(checkCounter2_4 == 2);

    }
    // Test if shared memory was cleaned up
    CHECK_THROWS_AS(
        boost::interprocess::shared_memory_object(boost::interprocess::open_only, "ipc_test_1", boost::interprocess::
            read_write), boost::interprocess::interprocess_exception);
}

TEST_CASE("IPC Test 2")
{
    auto ipc = std::make_shared<IPC>();
    auto tp = std::make_shared<SystemTimeProvider>();
    auto sched = std::make_shared<MultiThreadingScheduler>();
    auto sigHand = std::make_shared<SignalHandler>();
    auto agg = Aggregator::aggregate({ipc, tp, sched, sigHand});

    IPCOptions opt;
    opt.multiTarget = false; // setting to message queue
    auto publisher = ipc->publish<Test>("test2", opt);
    int counter1 = 0;
    Subscription sub = ipc->subscribe<Test>("test2",
                                            std::bind(&checkValue, std::placeholders::_1, std::ref(counter1)), opt);
    REQUIRE(sub.connected());
    SimpleRunner run(agg);
    REQUIRE(!run.runAllStages());
    test.val1 = 1;
    test.val2 = 1.2;
    test.val3 = Vector3(1, 2, 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    publisher.publish(test);
    publisher.publish(test);
    publisher.publish(test);
    publisher.publish(test);
    publisher.publish(test);
    publisher.publish(test);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    CHECK(counter1 == 6);

    test.val1 = 2;
    test.val2 = 2.2;
    test.val3 = Vector3(2, 3, 4);

    publisher.publish(test);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    CHECK(counter1 == 7);
}

namespace
{
    void
    onPacket(const Packet& p, int& packetCounter)
    {
        std::string test = "abc";
        CHECK_FALSE(p.getBuffer().compare(test));
        packetCounter++;
    }
}

TEST_CASE("Test Retry Functionality")
{
    auto ipc = std::make_shared<IPC>();
    auto sim = std::make_shared<MicroSimulator>();
    auto sched = std::make_shared<MultiThreadingScheduler>();
    auto sigHand = std::make_shared<SignalHandler>();
    auto dp = std::make_shared<DataPresentation>();
    auto agg = Aggregator::aggregate({sched, sim, sigHand}); // set sim as tp of sched
    auto agg2 = Aggregator::aggregate({ipc, sched, dp, sigHand}); // set sched as scheduler of ipc

    sim->stopOnWait();

    IPCParams params;
    params.retryPeriod = 100;

    int packetCounter = 0;
    int packetCounter2 = 0;

    IPCOptions opts;
    opts.multiTarget = true; //SHM
    opts.retry = true;

    IPCOptions opts2;
    opts2.multiTarget = true; //SHM
    opts2.retry = false;

    auto sub1 = ipc->subscribeOnPackets("test_packets",
                                        std::bind(onPacket, std::placeholders::_1, std::ref(packetCounter)), opts);
    auto sub2 = ipc->subscribeOnPackets("test_packets_2",
                                        std::bind(onPacket, std::placeholders::_1, std::ref(packetCounter2)), opts2);

    CHECK(!sub1.connected());
    CHECK(!sub2.connected());
    SimpleRunner run(agg2);
    REQUIRE(!run.runAllStages());

    std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

    sim->releaseWait(); //One retry
    std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

    CHECK(!sub1.connected());
    CHECK(!sub2.connected());

    auto pub = ipc->publishPackets("test_packets");
	auto pub2 = ipc->publishPackets("test_packets_2");

    sim->releaseWait(); //next retry, should be successful

    std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

    CHECK(sub1.connected());
    CHECK(!sub2.connected());

    pub.publish(Packet("abc"));
    pub2.publish(Packet("abc"));

    std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

    sim->releaseWait(); //next retry, does not do anything

    std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

    CHECK(sub1.connected());
    CHECK(!sub2.connected());

    CHECK(packetCounter == 1);
    CHECK(packetCounter2 == 0);

    pub.publish(Packet("abc"));
    pub2.publish(Packet("abc"));

    std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start


    CHECK(packetCounter == 2);
    CHECK(packetCounter2 == 0);
}
