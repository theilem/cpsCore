/*
 * AggregatableRunner.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: mircot
 */

#include "cpsCore/Aggregation/Aggregator.h"
#include "cpsCore/Synchronization/AggregatableRunner.h"

AggregatableRunner::AggregatableRunner()
{
}

bool
AggregatableRunner::runStage(RunStage stage)
{
	for (auto it : runnableObjects_)
	{
		if (it->run(stage))
			return true;
	}
	return false;
}

bool
AggregatableRunner::runAllStages()
{
	if (runStage(RunStage::INIT))
		return true;
	if (runStage(RunStage::NORMAL))
		return true;
	if (runStage(RunStage::FINAL))
		return true;
	return false;
}

void
AggregatableRunner::notifyAggregationOnUpdate(const Aggregator& agg)
{
	runnableObjects_ = agg.getAll<IRunnableObject>();
}
