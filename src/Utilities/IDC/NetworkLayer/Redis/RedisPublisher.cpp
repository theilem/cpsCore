/*
 * RedisPublisher.cpp
 *
 *  Created on: Jan 30, 2019
 *      Author: mirco
 */

#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisPublisher.h"
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Utilities/Time.hpp"

RedisPublisher::RedisPublisher(const RedisChannelParams& params) :
		channel_(params.channel_), auth_(params.auth_)
{
	client_.connect(params.hostIP_, params.port_,
			std::bind(&RedisPublisher::onConnectionEvent, this, std::placeholders::_1,
					std::placeholders::_2, std::placeholders::_3));

}

RedisPublisher::~RedisPublisher()
{
	client_.disconnect(false);
}

bool
RedisPublisher::publish(const Packet& packet)
{
	if (!client_.is_connected())
	{
		CPSLOG_ERROR << "Client is not connected, cannot send.";
		return false;
	}
	client_.publish(channel_, packet.getBuffer());
	client_.commit();
	return true;
}

void
RedisPublisher::startHandler()
{
}

void
RedisPublisher::onConnectionEvent(const std::string& host, std::size_t port,
		cpp_redis::connect_state status)
{
	CPSLOG_DEBUG << "Connection event " << static_cast<int>(status);
	if (status == cpp_redis::connect_state::ok)
	{
		if (auth_.empty())
		{
			CPSLOG_DEBUG << "Connection started";
			return;
		}

		CPSLOG_DEBUG << "Authenticating with " << auth_;
		auto future = client_.auth(auth_);
		client_.commit();

		auto status = future.wait_for(Seconds(1));
		if (status == std::future_status::timeout)
		{
			CPSLOG_ERROR << "Authentication timed out";
			return;
		}
		if (status == std::future_status::deferred)
		{
			CPSLOG_ERROR << "Authentication deferred";
			return;
		}
		auto reply = future.get();
		if (reply.is_error())
		{
			CPSLOG_ERROR << reply.error();
		}
	}

	if (status == cpp_redis::connect_state::dropped)
	{
		CPSLOG_ERROR << "Client disconnected from " << host << ": " << port;
	}
}

bool
RedisPublisher::isConnected() const
{
	return client_.is_connected();
}
