/*
 * FrameworkAPITest.cpp
 *
 *  Created on: Feb 4, 2019
 *      Author: mirco
 */

#include "TestInfo.h"

#include "cpsCore/Framework/api/FrameworkAPI.h"
#include "cpsCore/Framework/Helper.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/NetworkFactory.h"

namespace
{
class TestHelper: public Helper
{
public:
	TestHelper()
	{
		addFactory<NetworkFactory>();
	}
};
}

TEST_CASE("locking")
{
	auto lock = FrameworkAPI::lockAggregator();
	lock.unlock();
	auto lock2 = FrameworkAPI::lockAggregator();
}

TEST_CASE("Aggregation Merge")
{
	TestHelper helper;
	{
		Aggregator agg = helper.createAggregation(test_info::test_dir() + "Utilities/config/agg1.json");
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		globalAgg->merge(agg);
	}

	{
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		auto net1 = globalAgg->getOne<INetworkLayer>();
		CHECK(net1);
	}

	{
		Aggregator agg = helper.createAggregation(test_info::test_dir() + "Utilities/config/agg2.json");
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		globalAgg->merge(agg);
	}

	{
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		auto nets = globalAgg->getAll<INetworkLayer>();
		CHECK(nets.size() == 2);
	}

	{
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		auto netSerials = globalAgg->getAll<SerialNetworkLayer>();
		CHECK(netSerials.size() == 1);
		auto netRedis = globalAgg->getAll<RedisNetworkLayer>();
		CHECK(netRedis.size() == 1);

		CHECK(netSerials.front());
		CHECK(netRedis.front());
	}

	{
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		globalAgg->clear();
	}

	{
		auto lock = FrameworkAPI::lockAggregator();
		auto globalAgg = FrameworkAPI::getAggregator();
		auto netSerials = globalAgg->getAll<SerialNetworkLayer>();
		CHECK(netSerials.empty());
		auto netRedis = globalAgg->getAll<RedisNetworkLayer>();
		CHECK(netRedis.empty());
		auto nets = globalAgg->getAll<INetworkLayer>();
		CHECK(nets.empty());
	}

}

