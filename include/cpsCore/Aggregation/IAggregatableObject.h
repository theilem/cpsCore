/*
 * AggregatableObject.h
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_OBJECT_IAGGREGATABLEOBJECT_H_
#define UAVAP_CORE_OBJECT_IAGGREGATABLEOBJECT_H_

class Aggregator;

class IAggregatableObject
{

public:

	virtual
	~IAggregatableObject() = default;

	using TypeId = const char* const;

	virtual void
	notifyAggregationOnUpdate(const Aggregator& agg) = 0;
};



#endif /* UAVAP_CORE_OBJECT_IAGGREGATABLEOBJECT_H_ */
