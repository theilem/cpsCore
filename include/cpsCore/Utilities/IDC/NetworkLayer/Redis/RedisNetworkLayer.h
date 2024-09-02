/*
 * RedisNetworkLayer.h
 *
 *  Created on: Jan 25, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_IDC_NETWORKLAYER_REDIS_REDISNETWORKLAYER_H_
#define UAVAP_CORE_IDC_NETWORKLAYER_REDIS_REDISNETWORKLAYER_H_

#include <unordered_map>

#include "cpsCore/cps_object"
#include "cpsCore/Utilities/IDC/NetworkLayer/INetworkLayer.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Redis/RedisNetworkLayerParams.h"
#include "cpsCore/Utilities/Packet.h"


class IScheduler;

class RedisPublisher;

class RedisSubscriber;

class RedisNetworkLayer
		: public INetworkLayer,
		  public AggregatableObject<IScheduler>,
		  public IRunnableObject,
		  public ConfigurableObject<RedisNetworkLayerParams>
{
public:

	static constexpr TypeId typeId = "redis";

	bool
	sendPacket(const std::string& id, const Packet& packet) override;

	boost::signals2::connection
	subscribeOnPacket(const std::string& id, const OnPacket::slot_type& handle) override;

	bool
	run(RunStage stage) override;

	std::map<std::string, NetworkStats>
	getStats() const override;

	void
	resetStats(const std::string& id) override;

private:

	std::unordered_map<std::string, std::shared_ptr<RedisSubscriber>> subscribers_;
	std::unordered_map<std::string, std::shared_ptr<RedisPublisher>> publishers_;


};

#endif /* UAVAP_CORE_IDC_NETWORKLAYER_REDIS_REDISNETWORKLAYER_H_ */
