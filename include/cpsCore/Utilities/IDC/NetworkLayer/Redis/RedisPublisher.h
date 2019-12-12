/*
 * RedisPublisher.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mirco
 */

#ifndef PLUGIN_NETWORKLAYER_REDIS_REDISPUBLISHER_H_
#define PLUGIN_NETWORKLAYER_REDIS_REDISPUBLISHER_H_




#include <boost/signals2.hpp>
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisChannelParams.h"
#include "cpsCore/Utilities/IPC/detail/IPublisherImpl.h"


#include <cpp_redis/cpp_redis>

class RedisPublisher : public IPublisherImpl
{
public:

	RedisPublisher(const RedisChannelParams& params);

	~RedisPublisher();

	using OnPacket = boost::signals2::signal<void(const Packet&)>;

	bool
	publish(const Packet& packet) override;

	void
	startHandler();

	bool
	isConnected() const;
private:

	void
	onConnectionEvent(const std::string& host, std::size_t port, cpp_redis::connect_state status);

	cpp_redis::client client_;
	std::string channel_;
	std::string auth_;

};


#endif /* PLUGIN_NETWORKLAYER_REDIS_REDISPUBLISHER_H_ */
