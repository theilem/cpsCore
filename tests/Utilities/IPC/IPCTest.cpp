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
	double val2;
	Vector3 val3;
} test;

namespace dp
{
template<class Archive, typename Type>
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
	auto ipc = std::make_shared<IPC>();
	auto tp = std::make_shared<SystemTimeProvider>();
	auto sched = std::make_shared<MultiThreadingScheduler>();
	auto signalHandler = std::make_shared<SignalHandler>();
	auto agg = Aggregator::aggregate({ipc, tp, sched, signalHandler});

	auto publisher = ipc->publish<Test>("test");

	int counter1 = 0;
	int counter2 = 0;
	Subscription sub1 = ipc->subscribe<Test>("test",
											 boost::bind(&checkValue, _1, boost::ref(counter1)));
	Subscription sub2 = ipc->subscribe<Test>("test",
											 boost::bind(&checkValue, _1, boost::ref(counter2)));
	REQUIRE(sub1.connected());
	REQUIRE(sub2.connected());
	SimpleRunner run(agg);
	REQUIRE(!run.runAllStages());
	test.val1 = 1;
	test.val2 = 1.2;
	test.val3 = Vector3(1, 2, 3);

	CHECK(counter1 == 0);
	CHECK(counter2 == 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	CHECK(counter1 == 0);
	CHECK(counter2 == 0);
	publisher.publish(test);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	CHECK(counter1 == 1);
	CHECK(counter2 == 1);

	test.val1 = 2;
	test.val2 = 2.2;
	test.val3 = Vector3(2, 3, 4);

	publisher.publish(test);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	CHECK(counter1 == 2);
	CHECK(counter2 == 2);
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
retrySuccessCallback(const Subscription& sub, bool& called)
{
	CHECK(sub.connected());
	static int retryCounter = 0;
	called = true;
	retryCounter++;
	CHECK(retryCounter == 1);
}

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

	bool retryCalled = false;
	bool retryCalled2 = false;
	int packetCounter = 0;
	int packetCounter2 = 0;

	IPCOptions opts;
	opts.multiTarget = true; //SHM
	opts.retry = true;
	opts.retrySuccessCallback = std::bind(retrySuccessCallback, std::placeholders::_1,
										  std::ref(retryCalled));

	IPCOptions opts2;
	opts2.multiTarget = true; //SHM
	opts2.retry = false;
	opts2.retrySuccessCallback = std::bind(retrySuccessCallback, std::placeholders::_1,
										   std::ref(retryCalled2));

	auto sub1 = ipc->subscribeOnPackets("test_packets",
										std::bind(onPacket, std::placeholders::_1, std::ref(packetCounter)), opts);
	auto sub2 = ipc->subscribeOnPackets("test_packets",
										std::bind(onPacket, std::placeholders::_1, std::ref(packetCounter2)), opts2);

	CHECK(!sub1.connected());
	CHECK(!sub2.connected());
	SimpleRunner run(agg2);
	REQUIRE(!run.runAllStages());

	std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

	sim->releaseWait(); //One retry
	std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

	CHECK(!retryCalled);
	CHECK(!retryCalled2);

	auto pub = ipc->publishPackets("test_packets");

	sim->releaseWait(); //next retry, should be successful

	std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

	CHECK(retryCalled);
	CHECK(!retryCalled2);

	pub.publish(Packet("abc"));

	std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

	sim->releaseWait(); //next retry, does not do anything

	std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start

	CHECK(retryCalled);
	CHECK(!retryCalled2);

	CHECK(packetCounter == 1);
	CHECK(packetCounter2 == 0);

	pub.publish(Packet("abc"));

	std::this_thread::sleep_for(Milliseconds(10)); //Let scheduler start


	CHECK(packetCounter == 2);
	CHECK(packetCounter2 == 0);

}
