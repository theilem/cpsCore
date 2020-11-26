/*
 * ObjectHandleContainer.hpp
 *
 *  Created on: Jun 7, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_OBJECT_OBJECTHANDLECONTAINER_HPP_
#define UAVAP_CORE_OBJECT_OBJECTHANDLECONTAINER_HPP_

#include <cpsCore/Aggregation/Aggregator.h>

template<class ... Objects>
class ObjectHandleContainer;

template<class Object, class ... Others>
class ObjectHandleContainer<Object, Others...>
{
public:

	template<class Type>
	using PtrType = std::shared_ptr<Type>;
	template<class Type>
	using WeakPtrType = std::weak_ptr<Type>;

	template<class Ret>
	PtrType<Ret>
	get() const;

	template<class Ret>
	bool
	isSet() const;

	template<class Agg>
	void
	setFromAggregationIfNotSet(const Agg& agg);

	void
	setFromAggregationIfNotSet(const Aggregator& agg);

	template<class Ret>
	static constexpr bool
	canContain();

private:

	ObjectHandleContainer<Others...> others_;
	WeakPtrType<Object> object_;
};

template<>
class ObjectHandleContainer<>
{
public:

	template<class Type>
	using PtrType = std::shared_ptr<Type>;
	template<class Type>
	using WeakPtrType = std::weak_ptr<Type>;

	template<class Ret>
	inline std::shared_ptr<Ret>
	get() const
	{
		return nullptr;
	}

	template<class Ret>
	inline bool
	isSet() const
	{
		return false;
	}

	template<class Agg>
	inline void
	setFromAggregationIfNotSet(const Agg& agg)
	{
	}


	inline void
	setFromAggregationIfNotSet(const Aggregator& agg)
	{
	}


	template<class Ret>
	static constexpr bool
	canContain()
	{
		return false;
	}
};

template<class Object, class... Others>
template<class Ret>
constexpr bool
ObjectHandleContainer<Object, Others...>::canContain()
{
	if constexpr (std::is_base_of_v<Ret, Object>)
		return true;
	else
		return ObjectHandleContainer<Others...>::template canContain<Ret>();
}

template<class Object, class ... Others>
template<class Ret>
inline typename ObjectHandleContainer<Object, Others...>::template PtrType<Ret>
ObjectHandleContainer<Object, Others...>::get() const
{
	if constexpr (std::is_base_of_v<Ret, Object>)
		return object_.lock();
	else
		return others_.template get<Ret>();
}

template<class Object, class ... Others>
template<class Agg>
inline void
ObjectHandleContainer<Object, Others ...>::setFromAggregationIfNotSet(const Agg& agg)
{
	if (!object_.lock())
		object_ = agg.template getOne<Object>();
	others_.setFromAggregationIfNotSet(agg);
}

template<class Object, class ... Others>
inline void
ObjectHandleContainer<Object, Others ...>::setFromAggregationIfNotSet(const Aggregator& agg)
{
	if (!object_.lock())
		object_ = agg.getOne<Object>();
	others_.setFromAggregationIfNotSet(agg);
}


template<class Object, class ... Others>
template<class Ret>
inline bool
ObjectHandleContainer<Object, Others...>::isSet() const
{
	if constexpr (std::is_base_of_v<Ret, Object>)
		return object_.lock() != nullptr;
	else
		return others_.template isSet<Ret>();
}

#endif /* UAVAP_CORE_OBJECT_OBJECTHANDLECONTAINER_HPP_ */
