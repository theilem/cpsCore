//
// Created by mirco on 26.11.19.
//

#include <cpsCore/Utilities/TimeProvider/SystemTimeProvider.h>
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"
#include "TestInfo.h"
#include "cpsCore/Framework/StaticHelper.h"



TEST_CASE("Single Type Static Handler")
{
	StaticHelper<MicroSimulator> helper;

	Configuration config;
	config.add_child("micro_sim", Configuration());

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
	config.add_child("micro_sim", Configuration());

	auto agg = helper.createAggregation(config);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MicroSimulator>());
	CHECK_FALSE(agg.getOne<SignalHandler>());

	config.add_child("signal_handler", Configuration());
	config.add_child("system_time", Configuration());

	auto agg2 = helper.createAggregation(config);

	CHECK(agg2.getOne<IScheduler>());
	CHECK(agg2.getOne<MicroSimulator>());
	CHECK(agg2.getOne<SignalHandler>());
	CHECK(agg2.getOne<SystemTimeProvider>());

	CHECK(agg2.getAll<ITimeProvider>().size() == 2);
}
