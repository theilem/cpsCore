/*
 * Aggregator.h
 *
 *  Created on: Jun 29, 2017
 *      Author: mircot
 */

#ifndef UAVAP_CORE_OBJECT_AGGREGATOR_H_
#define UAVAP_CORE_OBJECT_AGGREGATOR_H_

#include "cpsCore/Aggregation/DynamicContainer/DynamicObjectContainer.h"
#include "cpsCore/Aggregation/IAggregatableObject.h"
#include <cpsCore/Synchronization/IRunnableObject.h>

#include <memory>
#include <vector>

class Aggregator
{
public:

	using ObjectContainer = DynamicObjectContainer;

	Aggregator();

	template<class Type>
	void
	add(std::shared_ptr<Type> obj);

	template<class Type>
	void
	add(std::vector<std::shared_ptr<Type>> objs);

	void
	add(std::shared_ptr<IAggregatableObject> obj);

	void
	add(std::vector<std::shared_ptr<IAggregatableObject>> objs);

	template<class...Types>
	void
	add(std::tuple<std::shared_ptr<Types>...> obj);

	void
	add(const ObjectContainer& obj);

	template<class Type>
	std::shared_ptr<Type>
	getOne(Type* self = nullptr) const;

	template<class Type>
	std::vector<std::shared_ptr<Type> >
	getAll(Type* self = nullptr) const;

	static Aggregator
	aggregate(std::vector<std::shared_ptr<IAggregatableObject> > aggregation);

	void
	merge(Aggregator& agg);

	void
	mergeInto(Aggregator& agg);

	/**
	 * @brief Clear the container. Will destroy all the objects.
	 */
	void
	clear();

	void
	cleanUp();

	size_t
	size() const;

	void
	signalRunBegin();

	void
	signalRunStageReached(const RunStage& runstage);

	bool
	runBegan() const;

	RunStage
	highestRunStage() const;

private:

	ObjectContainer container_;
	bool runBegan_;
	RunStage highestAchieved_;
};

template<class Type>
inline void
Aggregator::add(std::shared_ptr<Type> obj)
{
	if (auto aggObj = std::dynamic_pointer_cast<IAggregatableObject>(obj))
	{
		add(aggObj);
	}
}

template<class Type>
inline void
Aggregator::add(std::vector<std::shared_ptr<Type>> objs)
{
	for (auto it : objs)
	{
		add(it);
	}
}

template<class Type>
inline std::shared_ptr<Type>
Aggregator::getOne(Type* self) const
{
	return container_.template getOne<Type>(self);
}

template<class Type>
inline std::vector<std::shared_ptr<Type> >
Aggregator::getAll(Type* self) const
{
	return container_.template getAll<Type>(self);
}

template<class... Types>
void
Aggregator::add(std::tuple<std::shared_ptr<Types>...> obj)
{

	std::apply([this](auto...x)
			   { (this->add(x), ...); }, obj);

}

#endif /* UAVAP_CORE_OBJECT_AGGREGATOR_H_ */
