/*
 * Aggregator.cpp
 *
 *  Created on: Aug 6, 2017
 *      Author: mircot
 */
#include "cpsCore/Aggregation/Aggregator.h"
#include "cpsCore/Utilities/SignalHandler/SignalHandler.h"


Aggregator::Aggregator()
{
}

void
Aggregator::add(std::shared_ptr<IAggregatableObject> obj)
{
	container_.add(obj);
	container_.notifyAggregationOnUpdate(*this);
}

Aggregator
Aggregator::aggregate(const std::vector<std::shared_ptr<IAggregatableObject> >& aggregation)
{
	Aggregator agg;
	for (const auto& it : aggregation)
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
	container_.notifyAggregationOnUpdate(*this);
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
	SignalHandlerSingleton::getInstance().callSigHandlers(SIGTERM);
	clear();
}

void
Aggregator::add(const std::vector<std::shared_ptr<IAggregatableObject>>& objs)
{
	for (const auto& it : objs)
	{
		add(it);
	}
}



bool
Aggregator::empty() const
{
	return container_.empty();
}

std::size_t
Aggregator::size() const
{
	return container_.size();
}
