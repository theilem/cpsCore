//
// Created by mirco on 26.11.19.
//

#include "cpsCore/Utilities/TimeProvider/SystemTimeProvider.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"
#include "cpsCore/Framework/StaticHelper.h"
#include "cpsCore/Framework/StaticFactory.h"
#include "TestInfo.h"


TEST_CASE("Single Type Static Factory")
{
	using TestFactory = StaticFactory<IScheduler, false, MicroSimulator>;

	TestFactory factory;

	Configuration simConfig;
	simConfig.add_child("micro_sim", Configuration());

	CHECK(factory.typeId == IScheduler::typeId);
	CHECK(TestFactory::typeId == IScheduler::typeId);

	CHECK(is_static_factory<TestFactory>::value);

	auto microSim1 = factory.create("micro_sim");

	REQUIRE(microSim1);
	CHECK(std::dynamic_pointer_cast<MicroSimulator>(microSim1));
	CHECK(!std::dynamic_pointer_cast<MultiThreadingScheduler>(microSim1));

	auto microSim2 = factory.create(simConfig);

	REQUIRE(microSim2);
	CHECK(std::dynamic_pointer_cast<MicroSimulator>(microSim2));
	CHECK(!std::dynamic_pointer_cast<MultiThreadingScheduler>(microSim2));

	CPSLogger::LogLevelScope log(LogLevel::NONE);
	CHECK(!factory.create("micro_sim_does_not_exist"));
}

SCENARIO("Static Factory in Helper without multi")
{
	GIVEN("A Scheduler static factory in a helper")
	{
		using TestFactory = StaticFactory<IScheduler, false, MicroSimulator, MultiThreadingScheduler>;
		using TestHelper = StaticHelper<TestFactory>;

		WHEN("Only micro_sim in config")
		{

			Configuration simConfig;
			simConfig.add_child("micro_sim", Configuration());

			Configuration config;
			config.add_child("scheduler", simConfig);

			auto agg = TestHelper::createAggregation(config);

			THEN("No Thread scheduler")
			{
				CHECK(agg.getOne<IScheduler>());
				CHECK(agg.getOne<MicroSimulator>());
				CHECK(agg.getOne<ITimeProvider>());
				CHECK(!agg.getOne<MultiThreadingScheduler>());
			}
		}

		WHEN("micro_sim and thread in config")
		{

			Configuration simConfig;
			simConfig.add_child("micro_sim", Configuration());
			simConfig.add_child("thread", Configuration());

			Configuration config;
			config.add_child("scheduler", simConfig);

			CPSLogger::LogLevelScope log(LogLevel::NONE);
			auto agg = TestHelper::createAggregation(config);

			THEN("Only micro_sim in aggregation")
			{
				CHECK(agg.getOne<IScheduler>());
				CHECK(agg.getOne<MicroSimulator>());
				CHECK(agg.getOne<ITimeProvider>());
				CHECK(!agg.getOne<MultiThreadingScheduler>());
				CHECK(agg.getAll<IScheduler>().size() == 1);
			}
		}

		WHEN("thread and micro_sim in config")
		{

			Configuration simConfig;
			simConfig.add_child("thread", Configuration());
			simConfig.add_child("micro_sim", Configuration());

			Configuration config;
			config.add_child("scheduler", simConfig);

			CPSLogger::LogLevelScope log(LogLevel::NONE);
			auto agg = TestHelper::createAggregation(config);

			THEN("Only thread in aggregation")
			{
				CHECK(agg.getOne<IScheduler>());
				CHECK(!agg.getOne<MicroSimulator>());
				CHECK(!agg.getOne<ITimeProvider>());
				CHECK(agg.getOne<MultiThreadingScheduler>());
				CHECK(agg.getAll<IScheduler>().size() == 1);
			}
		}

	}
}

SCENARIO("Static Factory in Helper with multi")
{
	GIVEN("A Scheduler static factory in a helper")
	{
		using TestFactory = StaticFactory<IScheduler, true, MicroSimulator, MultiThreadingScheduler>;
		using TestHelper = StaticHelper<TestFactory>;

		WHEN("Only micro_sim in config")
		{

			Configuration simConfig;
			simConfig.add_child("micro_sim", Configuration());

			Configuration config;
			config.add_child("scheduler", simConfig);

			auto agg = TestHelper::createAggregation(config);

			THEN("No Thread scheduler")
			{
				CHECK(agg.getOne<IScheduler>());
				CHECK(agg.getOne<MicroSimulator>());
				CHECK(agg.getOne<ITimeProvider>());
				CHECK(!agg.getOne<MultiThreadingScheduler>());
			}
		}

		WHEN("micro_sim and thread in config")
		{

			Configuration simConfig;
			simConfig.add_child("micro_sim", Configuration());
			simConfig.add_child("thread", Configuration());

			Configuration config;
			config.add_child("scheduler", simConfig);

			auto agg = TestHelper::createAggregation(config);

			THEN("Both in aggregation")
			{
				CHECK(agg.getOne<IScheduler>());
				CHECK(agg.getOne<MicroSimulator>());
				CHECK(agg.getOne<ITimeProvider>());
				CHECK(agg.getOne<MultiThreadingScheduler>());
				CHECK(agg.getAll<IScheduler>().size() == 2);
			}
		}

	}


}
