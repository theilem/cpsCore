/*
 * RedisNetworkLayer.cpp
 *
 *  Created on: Jan 25, 2019
 *      Author: mirco
 */

#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisNetworkLayer.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisPublisher.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisSubscriber.h"
#include "cpsCore/Configuration/PropertyMapper.hpp"
#include "cpsCore/Utilities/Scheduler/IScheduler.h"


std::shared_ptr<RedisNetworkLayer>
RedisNetworkLayer::create(const Configuration& config)
{
	auto rnl = std::make_shared<RedisNetworkLayer>();
	rnl->configure(config);
	return rnl;
}

bool
RedisNetworkLayer::configure(const Configuration& config)
{
	PropertyMapper<Configuration> pm(config);

	Configuration sub;
	pm.add("sub", sub, false);

	for (const auto& it : sub)
	{
		RedisChannelParams params;
		if (!params.configure(it.second))
			return false;

		subscribers_.emplace(it.first, std::make_shared<RedisSubscriber>(params));
	}

	Configuration pub;
	pm.add("pub", pub, false);

	for (const auto& it : pub)
	{
		RedisChannelParams params;
		if (!params.configure(it.second))
			return false;

		publishers_.emplace(it.first, std::make_shared<RedisPublisher>(params));
	}

	return pm.map();

}

bool
RedisNetworkLayer::sendPacket(const std::string& id, const Packet& packet)
{
	auto it = publishers_.find(id);
	if (it == publishers_.end())
	{
		CPSLOG_ERROR << "Connection id not specified: " << id;
		return false;
	}

	return it->second->publish(packet);
}

boost::signals2::connection
RedisNetworkLayer::subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle)
{
	auto it = subscribers_.find(id);
	if (it == subscribers_.end())
	{
		CPSLOG_ERROR << "Connection id not specified: " << id;
		return boost::signals2::connection();
	}

	return it->second->subscribe(handle);
}

bool
RedisNetworkLayer::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!isSet<IScheduler>())
		{
			CPSLOG_ERROR << "RedisNetworkLayer scheduler missing";
			return true;
		}
		break;
	}
	case RunStage::NORMAL:
	{
		auto sched = get<IScheduler>();
		for (const auto& it : subscribers_)
		{
			sched->schedule(std::bind(&RedisSubscriber::start, it.second), Milliseconds(0));
		}

		for (const auto& it : publishers_)
		{
			sched->schedule(std::bind(&RedisPublisher::startHandler, it.second), Milliseconds(0));
		}
		break;
	}
	default:
		break;
	}
	return false;
}
