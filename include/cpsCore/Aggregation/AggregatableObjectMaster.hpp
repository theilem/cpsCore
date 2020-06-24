//
// Created by mirco on 19.06.20.
//

#ifndef CPSCORE_AGGREGATABLEOBJECTMASTER_H
#define CPSCORE_AGGREGATABLEOBJECTMASTER_H

#include "cpsCore/Aggregation/AggregatableObject.hpp"

template<class ... Objects>
class AggregatableObjectMaster : public AggregatableObject<Objects...>
{
public:

	void
	notifyAggregationOnUpdate(const Aggregator& agg) override;

protected:

	const Aggregator* aggregator_;

};

template<class...Objects>
void
AggregatableObjectMaster<Objects...>::notifyAggregationOnUpdate(const Aggregator& agg)
{
	this->container_.setFromAggregationIfNotSet(agg);
	aggregator_ = &agg;
}


#endif //CPSCORE_AGGREGATABLEOBJECTMASTER_H
