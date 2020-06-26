/*
 * Aggregator.cpp
 *
 *  Created on: Aug 6, 2017
 *      Author: mircot
 */
#include "cpsCore/Aggregation/Aggregator.h"
#include "cpsCore/Logging/CPSLogger.h"


Aggregator::Aggregator() : runBegan_(false), highestAchieved_(RunStage::INIT)
{
}

void
Aggregator::add(std::shared_ptr<IAggregatableObject> obj)
{
	container_.add(obj);
	container_.notifyAggregationOnUpdate(*this);
}

void
Aggregator::add(std::vector<std::shared_ptr<IAggregatableObject>> objs)
{
	for (auto it : objs)
	{
		add(it);
	}
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
}

size_t
Aggregator::size() const
{
	return container_.size();
}

void
Aggregator::signalRunBegin()
{
	runBegan_ = true;
}

void
Aggregator::signalRunStageReached(const RunStage& runstage)
{
	if (runstage > highestAchieved_)
	{
		highestAchieved_ = runstage;
	}
	else
	{
		//TODO Give RunStages string representation
		CPSLOG_WARN << "Signaled RunStage " << (int) runstage << " has already been reached.";
	}
}

bool
Aggregator::runBegan() const
{
	return runBegan_;
}

RunStage
Aggregator::highestRunStage() const
{
	return highestAchieved_;
}
