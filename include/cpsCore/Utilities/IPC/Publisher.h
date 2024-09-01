/*
 * Publisher.h
 *
 *  Created on: Jul 18, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_PUBLISHER_H_
#define UAVAP_CORE_IPC_PUBLISHER_H_

#include "cpsCore/Utilities/IPC/detail/IPublisherImpl.h"
#include "cpsCore/Logging/CPSLogger.h"

#include <memory>

template <typename Pub>
class Publisher
{
public:
    Publisher() = default;

    Publisher(std::shared_ptr<IPublisherImpl> impl, std::function<Packet(const Pub&)> forwarding);

    bool
    publish(const Pub& obj);

private:
    std::weak_ptr<IPublisherImpl> publisherImpl_;

    std::function<Packet(const Pub&)> forwarding_;
};

template <typename Pub>
inline
Publisher<Pub>::Publisher(std::shared_ptr<IPublisherImpl> impl, std::function<Packet(const Pub&)> forwarding) :

    publisherImpl_(impl), forwarding_(forwarding)
{
}

template <typename Pub>
inline bool
Publisher<Pub>::publish(const Pub& obj)
{
    if (auto impl = publisherImpl_.lock())
    {
        return impl->publish(forwarding_(obj));
    }
    CPSLOG_ERROR << "Cannot publish because Impl is not set.";
    return false;
}

#endif /* UAVAP_CORE_IPC_PUBLISHER_H_ */
