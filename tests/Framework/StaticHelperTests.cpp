//
// Created by mirco on 26.11.19.
//

#include <cpsCore/Utilities/TimeProvider/SystemTimeProvider.h>
#include <cpsCore/Utilities/Scheduler/SchedulerFactory.h>
#include <cpsCore/Utilities/Scheduler/MicroSimulator.h>
#include <cpsCore/Utilities/SignalHandler/SignalHandler.h>
#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Framework/StaticHelper.h>



TEST_CASE("Single Type Static Handler")
{
	StaticHelper<MicroSimulator> helper;

	Configuration config;
	config["micro_sim"] = Configuration();

	auto agg = helper.createAggregation(config);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MicroSimulator>());
	CHECK_FALSE(agg.getOne<SignalHandler>());

	auto agg2 = helper.createAggregation(Configuration());

	CHECK_FALSE(agg2.getOne<IScheduler>());
	CHECK_FALSE(agg2.getOne<MicroSimulator>());
	CHECK_FALSE(agg2.getOne<SignalHandler>());
}

TEST_CASE("Multi Type Static Handler")
{
	StaticHelper<MicroSimulator, SignalHandler, SystemTimeProvider> helper;

	Configuration config;
	config["micro_sim"] = Configuration();

	auto agg = helper.createAggregation(config);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MicroSimulator>());
	CHECK_FALSE(agg.getOne<SignalHandler>());

	config["signal_handler"] = Configuration();
	config["system_time"] = Configuration();

	auto agg2 = helper.createAggregation(config);

	CHECK(agg2.getOne<IScheduler>());
	CHECK(agg2.getOne<MicroSimulator>());
	CHECK(agg2.getOne<SignalHandler>());
	CHECK(agg2.getOne<SystemTimeProvider>());

	CHECK(agg2.getAll<ITimeProvider>().size() == 2);
}

TEST_CASE("Default static helper")
{
	using Defaults = StaticHelper<SignalHandler, SchedulerFactory>;
	using HelperWithDefaults = StaticHelper<Defaults, SystemTimeProvider>;

	Configuration config;

	auto agg = HelperWithDefaults::createAggregation(config);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MultiThreadingScheduler>());
	CHECK(agg.getOne<SignalHandler>());
	CHECK_FALSE(agg.getOne<ITimeProvider>());

	config["system_time"] = Configuration();

	auto agg2 = HelperWithDefaults::createAggregation(config);

	CHECK(agg2.getOne<IScheduler>());
	CHECK(agg2.getOne<MultiThreadingScheduler>());
	CHECK(agg2.getOne<SignalHandler>());
	CHECK(agg2.getOne<ITimeProvider>());

	CHECK(agg2.getOne<MultiThreadingScheduler>()->getParams().priority() == MultiThreadingSchedulerParams().priority());

	Configuration schedConfig;
	Configuration multiThreadSchedConfig;
	multiThreadSchedConfig["priority"] = 10;
	schedConfig["thread"] = multiThreadSchedConfig;
	config["scheduler"] = schedConfig;

	auto agg3 = HelperWithDefaults::createAggregation(config);

	CHECK(agg3.getOne<IScheduler>());
	REQUIRE(agg3.getOne<MultiThreadingScheduler>());
	CHECK(agg3.getOne<SignalHandler>());
	CHECK(agg3.getOne<ITimeProvider>());

	CHECK(agg3.getOne<MultiThreadingScheduler>()->getParams().priority() == 10);
}
