/*
 * RedisSubscriber.cpp
 *
 *  Created on: Jan 30, 2019
 *      Author: mirco
 */

#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisSubscriber.h"
#include "cpsCore/Logging/CPSLogger.h"

RedisSubscriber::RedisSubscriber(const RedisChannelParams& params, const RedisHostParams& host)
{
	subscriber_.connect(host.ip(), host.port(),
			[](const std::string& host, std::size_t port, cpp_redis::connect_state status)
			{	if (status == cpp_redis::connect_state::dropped)
				{	CPSLOG_ERROR << "Client disconnected from " << host << ": " << port;}});
	if (!host.auth().empty())
		subscriber_.auth(host.auth());
	subscriber_.subscribe(params.channel(),
			std::bind(&RedisSubscriber::onChannel, this, std::placeholders::_1,
					std::placeholders::_2));
}

RedisSubscriber::~RedisSubscriber()
{
	subscriber_.disconnect(false);
}

boost::signals2::connection
RedisSubscriber::subscribe(const OnPacket::slot_type& slot)
{
	return onPacket_.connect(slot);
}

void
RedisSubscriber::start()
{
	subscriber_.commit();
}

void
RedisSubscriber::onChannel(const std::string& channel, const std::string& message)
{
	onPacket_(Packet(message));
}

void
RedisSubscriber::cancel()
{
	subscriber_.disconnect(false);
}
