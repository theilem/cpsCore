/*
 * RedisSubscriber.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mirco
 */

#ifndef PLUGIN_NETWORKLAYER_REDIS_REDISSUBSCRIBER_H_
#define PLUGIN_NETWORKLAYER_REDIS_REDISSUBSCRIBER_H_


#include <boost/signals2.hpp>
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisChannelParams.h"

#include <cpp_redis/cpp_redis>
#include "cpsCore/Utilities/IPC/detail/ISubscriptionImpl.h"


class RedisSubscriber : public ISubscriptionImpl
{
public:

	RedisSubscriber(const RedisChannelParams& params, const RedisHostParams& host);

	~RedisSubscriber() override;

	using OnPacket = boost::signals2::signal<void(const Packet&)>;

	boost::signals2::connection
	subscribe(const OnPacket::slot_type& slot) override;

	void
	cancel() override;

	void
	start() override;

	bool
	connect() override;

	bool
	connected() const override;

private:

	void
	onChannel(const std::string& channel, const std::string& message);

	RedisChannelParams params_;
	RedisHostParams host_;

	cpp_redis::subscriber subscriber_;

	OnPacket onPacket_;

};


#endif /* PLUGIN_NETWORKLAYER_REDIS_REDISSUBSCRIBER_H_ */
