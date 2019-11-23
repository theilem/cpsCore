/*
 * AggregatableObject.hpp
 *
 *  Created on: Jun 7, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_OBJECT_AGGREGATABLEOBJECT_HPP_
#define UAVAP_CORE_OBJECT_AGGREGATABLEOBJECT_HPP_

#include "cpsCore/Aggregation/IAggregatableObject.h"
#include "cpsCore/Aggregation/ObjectHandleContainer.hpp"
#include "cpsCore/Logging/CPSLogger.h"

template<class ... Objects>
class AggregatableObject: public IAggregatableObject
{
public:

	template<class Type>
	using PtrType = std::shared_ptr<Type>;
	template<class Type>
	using WeakPtrType = std::weak_ptr<Type>;

	template <class Agg>
	void
	notifyAggregationOnUpdate(const Agg& agg);

	void
	notifyAggregationOnUpdate(const Aggregator& agg) override;

	template<class Ret>
	PtrType<Ret>
	get() const;

	template<class...Checks>
	bool
	isSet() const;

	template<class...Checks>
	bool
	checkIsSet() const;

protected:

	template<class Ret>
	bool
	isSetImpl() const;

	template<class Check>
	bool
	checkIsSetImpl() const;

	ObjectHandleContainer<Objects...> container_;

};


template<class ... Objects>
inline void
AggregatableObject<Objects...>::notifyAggregationOnUpdate(const Aggregator& agg)
{
	container_.setFromAggregationIfNotSet(agg);
}

template<class ... Objects>
template<class Agg>
void
AggregatableObject<Objects...>::notifyAggregationOnUpdate(const Agg& agg)
{
	container_.setFromAggregationIfNotSet(agg);
}

template<class ... Objects>
template<class Ret>
inline typename AggregatableObject<Objects...>::template PtrType<Ret>
AggregatableObject<Objects...>::get() const
{
	return container_.template get<Ret>();
}



template<class ... Objects>
template<class Ret>
inline bool
AggregatableObject<Objects...>::isSetImpl() const
{
	return container_.template isSet<Ret>();
}

template<class ... Objects>
template<class ... Checks>
inline bool
AggregatableObject<Objects...>::isSet() const
{
	return (... && (this->template isSetImpl<Checks>()));
}

template<class ... Objects>
template<class Check>
inline bool
AggregatableObject<Objects...>::checkIsSetImpl() const
{
	if (isSet<Check>())
		return true;
	CPSLOG_ERROR << Check::typeId << " Missing";
	return false;
}

template<class ... Objects>
template<class ... Checks>
inline bool
AggregatableObject<Objects...>::checkIsSet() const
{
	return (... && (this->template checkIsSetImpl<Checks>()));
}


#endif /* UAVAP_CORE_OBJECT_AGGREGATABLEOBJECT_HPP_ */
