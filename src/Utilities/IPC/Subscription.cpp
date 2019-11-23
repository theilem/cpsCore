/*
 * Subscription.cpp
 *
 *  Created on: Aug 3, 2017
 *      Author: mircot
 */
#include "cpsCore/Utilities/IPC/Subscription.h"

Subscription::Subscription()
{
}

Subscription::Subscription(std::shared_ptr<ISubscriptionImpl> impl,
		const boost::signals2::connection& con) :
		subscriptionImpl_(impl), connection_(con)
{
}

void
Subscription::cancel()
{
	if (connection_.connected())
		connection_.disconnect();
}

bool
Subscription::connected() const
{
	return subscriptionImpl_.lock() != nullptr;
}

void
Subscription::connect(std::shared_ptr<ISubscriptionImpl> impl,
		const boost::signals2::connection& con)
{
	subscriptionImpl_ = impl;
	connection_ = con;
}
