//
// Created by mirco on 26.11.19.
//

#include "cpsCore/Utilities/TimeProvider/SystemTimeProvider.h"
#include "cpsCore/Utilities/Scheduler/MicroSimulator.h"
#include "cpsCore/Utilities/Scheduler/MultiThreadingScheduler.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"
#include "TestInfo.h"


TEST_CASE("Aggregator tuple add")
{
	auto tuple = std::make_tuple(std::make_shared<MicroSimulator>(), std::make_shared<MultiThreadingScheduler>());

	Aggregator agg;

	agg.add(tuple);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MicroSimulator>());
	CHECK(agg.getOne<ITimeProvider>());
	CHECK(agg.getOne<MultiThreadingScheduler>());
	CHECK(agg.getAll<IScheduler>().size() == 2);
}


TEST_CASE("Aggregator vector add")
{
	std::vector<std::shared_ptr<IAggregatableObject>> vector = {std::make_shared<MicroSimulator>(),
																std::make_shared<MultiThreadingScheduler>()};

	Aggregator agg;

	agg.add(vector);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MicroSimulator>());
	CHECK(agg.getOne<ITimeProvider>());
	CHECK(agg.getOne<MultiThreadingScheduler>());
	CHECK(agg.getAll<IScheduler>().size() == 2);
}



TEST_CASE("Aggregator vector add templated")
{
	std::vector<std::shared_ptr<IScheduler>> vector = {std::make_shared<MicroSimulator>(),
																std::make_shared<MultiThreadingScheduler>()};

	Aggregator agg;

	agg.add(vector);

	CHECK(agg.getOne<IScheduler>());
	CHECK(agg.getOne<MicroSimulator>());
	CHECK(agg.getOne<ITimeProvider>());
	CHECK(agg.getOne<MultiThreadingScheduler>());
	CHECK(agg.getAll<IScheduler>().size() == 2);
}
