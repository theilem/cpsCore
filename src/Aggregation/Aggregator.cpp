/*
 * Aggregator.cpp
 *
 *  Created on: Aug 6, 2017
 *      Author: mircot
 */
#include "cpsCore/Logging/CPSLogger.h"
#include "cpsCore/Aggregation/SignalHandler.h"
#include "uavAP/Core/Object/Aggregator.h"


Aggregator::Aggregator()
{
	add(std::make_shared<SignalHandler>(*this));
	CPSLOG_DEBUG << "Aggregator with signal handler created";
}

void
Aggregator::add(std::shared_ptr<IAggregatableObject> obj)
{
	container_.add(obj);
	container_.notifyAggregationOnUpdate(*this);
}

Aggregator
Aggregator::aggregate(std::vector<std::shared_ptr<IAggregatableObject> > aggregation)
{
	Aggregator agg;
	for (auto it : aggregation)
	{
		agg.add(it);
	}
	return agg;
}

void
Aggregator::add(const ObjectContainer& obj)
{
	container_.add(obj);
}

void
Aggregator::merge(Aggregator& agg)
{
	agg.mergeInto(*this);
}

void
Aggregator::mergeInto(Aggregator& agg)
{
	agg.add(container_);
	container_.clear();
}

void
Aggregator::clear()
{
	container_.clear();
}

void
Aggregator::cleanUp()
{
	clear();

	SignalHandlerSingleton::getInstance().joinHandler();
}
