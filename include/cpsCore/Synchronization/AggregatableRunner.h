/*
 * AggregatableRunner.h
 *
 *  Created on: Jul 1, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_RUNNER_AGGREGATABLERUNNER_H_
#define UAVAP_CORE_RUNNER_AGGREGATABLERUNNER_H_
#include "cpsCore/Aggregation/IAggregatableObject.h"
#include "cpsCore/Synchronization/IRunnableObject.h"
#include <memory>
#include <vector>

class Aggregator;

class AggregatableRunner: public IAggregatableObject
{

public:

	static constexpr TypeId typeId = "runner";

	AggregatableRunner();

	bool
	runStage(RunStage stage);

	bool
	runAllStages();

	void
	notifyAggregationOnUpdate(const Aggregator& agg);

private:

	std::vector<std::shared_ptr<IRunnableObject> > runnableObjects_;

};

#endif /* UAVAP_CORE_RUNNER_AGGREGATABLERUNNER_H_ */
