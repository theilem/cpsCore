/*
 * Subscription.h
 *
 *  Created on: Aug 3, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_IPC_SUBSCRIPTION_H_
#define UAVAP_CORE_IPC_SUBSCRIPTION_H_

#include "cpsCore/Utilities/IPC/detail/ISubscriptionImpl.h"

class Subscription
{
public:

	Subscription() = default;

	Subscription(std::shared_ptr<ISubscriptionImpl> impl, const boost::signals2::connection& con);

	void
	cancel();

	bool
	connected() const;

	void
	connect(std::shared_ptr<ISubscriptionImpl> impl, const boost::signals2::connection& con);

private:

	std::weak_ptr<ISubscriptionImpl> subscriptionImpl_;

	boost::signals2::connection connection_;

};

#endif /* UAVAP_CORE_IPC_SUBSCRIPTION_H_ */
