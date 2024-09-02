/*
 * SerialIDC.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: mircot
 */
#include "cpsCore/Configuration/PropertyMapper.hpp"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialNetworkLayer.h"
#include "cpsCore/Utilities/Scheduler/IScheduler.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"

bool
SerialNetworkLayer::sendPacket(const std::string& id, const Packet& packet)
{
	auto it = handler_.find(id);
	if (it == handler_.end())
	{
		CPSLOG_ERROR << "Connection id not specified: " << id;
		return false;
	}

	return it->second->sendPacket(packet);
}

boost::signals2::connection
SerialNetworkLayer::subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle)
{
	auto it = handler_.find(id);
	if (it == handler_.end())
	{
		CPSLOG_ERROR << "Connection id not specified: " << id;
		return boost::signals2::connection();
	}

	return it->second->subscribeOnPackets(handle);
}

bool
SerialNetworkLayer::run(RunStage stage)
{
	switch (stage)
	{
	case RunStage::INIT:
	{
		if (!checkIsSet<IScheduler, SignalHandler>())
		{
			CPSLOG_ERROR << "SerialNetworkLayer missing Dependencies";
			return true;
		}
		for (const auto& it : params.ports())
		{
			try
			{
				handler_.emplace(it.first, std::make_shared<SerialHandler>(it.second));
			}
			catch (const boost::system::system_error& err)
			{
				CPSLOG_ERROR << "An error occurred when creating SerialHandler for " << it.first << ": " << err.what();
				return true;
			}
		}
		break;
	}
	case RunStage::NORMAL:
	{
		auto sched = get<IScheduler>();
		for (const auto& it : handler_)
		{
			sched->schedule(std::bind(&SerialHandler::startHandler, it.second), Milliseconds(0));
		}

		if (auto sh = get<SignalHandler>())
		{
 			sh->subscribeOnSigint(
					std::bind(&SerialNetworkLayer::onSigInt, this, std::placeholders::_1));
		}
		break;
	}
	default:
		break;
	}
	return false;
}

std::map<std::string, NetworkStats>
SerialNetworkLayer::getStats() const
{
	std::map<std::string, NetworkStats> stats;
	for (const auto& it : handler_)
	{
		if (it.second)
			stats.insert(std::make_pair(it.first, it.second->getStats()));
	}
	return stats;
}

void
SerialNetworkLayer::resetStats(const std::string& id)
{
	auto handler = handler_.find(id);
	if (handler != handler_.end())
	{
		handler->second->resetStats();
	}
	else
	{
		CPSLOG_ERROR << "Cannot reset stats for: " << id;
	}
}

void
SerialNetworkLayer::onSigInt(int sig)
{
	if (sig == SIGINT || sig == SIGTERM)
	{
		CPSLOG_DEBUG << "Caught sigint. Canceling Serial handlers.";
		for (const auto& it : handler_)
		{
			it.second->cancelHandler();
		}
		handler_.clear();
	}
}

SerialNetworkLayer::~SerialNetworkLayer()
{
}

