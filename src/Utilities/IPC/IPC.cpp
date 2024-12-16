/*
 * IPC.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: mircot
 */

#include <cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisPublisher.h>
#include <cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisSubscriber.h>
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"
#include "cpsCore/Utilities/IPC/IPC.h"
#include "cpsCore/Utilities/IPC/detail/MessageQueuePublisherImpl.h"
#include "cpsCore/Utilities/IPC/detail/MessageQueueSubscriptionImpl.h"
#include "cpsCore/Utilities/IPC/detail/SharedMemoryPublisherImpl.h"

IPC::~IPC()
{
	std::lock_guard<std::mutex> lock(subscribeMutex_);
	for (auto& it: subscriptions_)
	{
		it.second->cancel();
	}
}

bool
IPC::run(RunStage stage)
{
	switch (stage)
	{
		case RunStage::INIT:
		{
			if (!isSet<IScheduler>())
			{
				CPSLOG_WARN << "IPC is missing Scheduler. Subscription functionality disabled.";
			}
			break;
		}
		case RunStage::NORMAL:
		{
			if (auto sh = get<SignalHandler>())
			{
				CPSLOG_DEBUG << "IPC subscribing on SIGINT";
				sh->subscribeOnSigint([this](int sig)
									  { sigintHandler(sig); });
			}

			if (params.retryPeriod() != 0)
			{
				if (auto sched = get<IScheduler>())
				{
					CPSLOG_DEBUG << "Schedule retries";
					sched->schedule([this]
									{ retrySubscriptions(); }, Milliseconds(0),
									Milliseconds(params.retryPeriod()), "ipc_retry_subscriptions");
				}
			}
			break;
		}
		default:
			break;
	}
	return false;
}

Publisher<Packet>
IPC::publishPackets(const std::string& id, const IPCOptions& options)
{
	if (params.useRedis())
	{
		return {publishOnRedis(id, params.maxPacketSize()),
				[](const Packet& p)
				{ return p; }};
	}
	if (options.multiTarget)
	{
		return {publishOnSharedMemory(id, params.maxPacketSize()),
				[](const Packet& p)
				{ return p; }};
	}
	else
	{
		return {
				publishOnMessageQueue(id, params.maxPacketSize(), params.maxNumPackets()),
				[](const Packet& p)
				{ return p; }};
	}
}

std::shared_ptr<IPublisherImpl>
IPC::publishOnSharedMemory(const std::string& id, unsigned maxPacketSize)
{
	auto impl = std::make_shared<SharedMemoryPublisherImpl>(id, maxPacketSize);
	publications_.push_back(impl);
	return impl;
}

std::shared_ptr<IPublisherImpl>
IPC::publishOnMessageQueue(const std::string& id, unsigned maxPacketSize, unsigned numPackets)
{
	std::shared_ptr<MessageQueuePublisherImpl> impl;
	try
	{
		impl = std::make_shared<MessageQueuePublisherImpl>(id, numPackets, maxPacketSize);
	} catch (boost::interprocess::interprocess_exception&)
	{
		CPSLOG_WARN << "Message queue with id: " << id << " already exists. Creating new.";
		boost::interprocess::message_queue::remove(id.c_str());
		impl = std::make_shared<MessageQueuePublisherImpl>(id, numPackets, maxPacketSize);
	}

	publications_.push_back(impl);
	return impl;
}

std::shared_ptr<IPublisherImpl>
IPC::publishOnRedis(const std::string& id, unsigned)
{
	RedisHostParams localhost;
	localhost.ip = "127.0.0.1";
	localhost.port = 6379;
	localhost.auth = "";
	RedisChannelParams params;
	params.channel.setValue(id);
	auto impl = std::make_shared<RedisPublisher>(params, localhost);
	publications_.push_back(impl);

	return impl;
}

void
IPC::sigintHandler(int sig)
{
	if (sig == SIGINT || sig == SIGTERM)
	{
		CPSLOG_DEBUG << "Caught signal " << sig << ". Deleting subscriptions and publications.";
		CPSLogger::instance()->flush();
		for (const auto& sub: subscriptions_)
		{
			sub.second->cancel();
		}

		subscriptions_.clear();
		publications_.clear();
	}
}

Subscription
IPC::subscribeOnPackets(const std::string& id, const std::function<void
		(const Packet&)>& slot, const IPCOptions& options)
{
	std::lock_guard<std::mutex> lock(subscribeMutex_);

	auto subscription = subscriptions_.find(id);
	if (subscription != subscriptions_.end())
	{
		auto impl = subscription->second;
		auto con = impl->subscribe(slot);
		return {subscription->second, con};
	}

	auto sub = subscribeOnPacketsImpl(id, options);
	if (sub->connected())
	{
		if (auto sched = get<IScheduler>())
		{
			sched->schedule([sub]
							{ sub->start(); },
							Milliseconds(0), "start_shared_memory_subscription_" + id);
		}
		else
		{
			CPSLOG_ERROR << "Scheduler missing. Cannot start subscription.";
		}
	}

	auto connection = sub->subscribe(slot);
	subscriptions_.insert(std::make_pair(id, sub));

	if (options.retry)
	{
		LockGuard l(retryVectorMutex_);
		CPSLOG_DEBUG << "Queuing " << id << " for retry";
		retryVector_.push_back(id);
	}

	return {sub, connection};
}

std::shared_ptr<ISubscriptionImpl>
IPC::subscribeOnPacketsImpl(const std::string& id, const IPCOptions& options)
{
	if (params.useRedis())
	{
		return subscribeOnRedis(id);
	}
	else
	{
		if (options.multiTarget)
		{
			return subscribeOnSharedMemory(id);
		}
		else
		{
			return subscribeOnMessageQueue(id);
		}
	}
}

std::shared_ptr<ISubscriptionImpl>
IPC::subscribeOnSharedMemory(const std::string& id)
{
	return std::make_shared<SharedMemorySubscriptionImpl>(id);
}

std::shared_ptr<ISubscriptionImpl>
IPC::subscribeOnMessageQueue(const std::string& id)
{
	return std::make_shared<MessageQueueSubscriptionImpl>(id, params.maxPacketSize());
}

std::shared_ptr<ISubscriptionImpl>
IPC::subscribeOnRedis(const std::string& id)
{
	RedisHostParams localhost;
	localhost.ip = "127.0.0.1";
	localhost.port = 6379;
	localhost.auth = "";
	RedisChannelParams channelParams;
	channelParams.channel.setValue(id);
	return std::make_shared<RedisSubscriber>(channelParams, localhost);
}

void
IPC::retrySubscriptions()
{
	LockGuard l(retryVectorMutex_);

	for (const auto& id: retryVector_)
	{
		auto sub = subscriptions_.find(id);
		if (sub != subscriptions_.end())
		{
			if (!sub->second->connected())
			{
				CPSLOG_DEBUG << "Retrying " << id;
				if (sub->second->connect())
				{
					sub->second->start();
				}
			}
		}
	}
}