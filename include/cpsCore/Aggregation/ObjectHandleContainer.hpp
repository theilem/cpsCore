/*
 * ObjectHandleContainer.hpp
 *
 *  Created on: Jun 7, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_OBJECT_OBJECTHANDLECONTAINER_HPP_
#define UAVAP_CORE_OBJECT_OBJECTHANDLECONTAINER_HPP_

#include <cpsCore/Aggregation/Aggregator.h>

#include "MultiObjectHandle.hpp"

template <class... Objects>
class ObjectHandleContainer;

template <class Object, class... Others>
class ObjectHandleContainer<Object, Others...>
{
public:
    template <class Type>
    using PtrType = std::shared_ptr<Type>;
    template <class Type>
    using WeakPtrType = std::weak_ptr<Type>;

    using ObjectType = std::conditional_t<is_multi_object_handle<Object>::value, Object, WeakPtrType<Object>>;

    // ========== Primary template ==========
    template <typename Type, bool = is_multi_object_handle<Type>::value>
    struct GetTypeImpl {
        using type = PtrType<Type>;
    };

    // ========== Specialization for multi-object handle ==========
    template <typename Type>
    struct GetTypeImpl<Type, true> {
        using type = std::vector<PtrType<typename Type::Type>>;
    };

    // ========== Alias ==========
    template <typename Type>
    using GetType = typename GetTypeImpl<Type>::type;

    template <class Ret>
    GetType<Ret>
    get() const;

    template <class Ret>
    bool
    isSet() const;

    template <class Agg>
    void
    setFromAggregationIfNotSet(const Agg& agg);

    void
    setFromAggregationIfNotSet(const Aggregator& agg);

    template <class Ret>
    static constexpr bool
    canContain();

private:
    ObjectHandleContainer<Others...> others_;
    ObjectType object_;
};

template <>
class ObjectHandleContainer<>
{
public:
    template <class Type>
    using PtrType = std::shared_ptr<Type>;
    template <class Type>
    using WeakPtrType = std::weak_ptr<Type>;
    template <class Type>
    using GetType = PtrType<Type>;

    template <class Ret>
    inline std::shared_ptr<Ret>
    get() const
    {
        return nullptr;
    }

    template <class Ret>
    inline bool
    isSet() const
    {
        return false;
    }

    template <class Agg>
    inline void
    setFromAggregationIfNotSet(const Agg& agg)
    {
    }


    inline void
    setFromAggregationIfNotSet(const Aggregator& agg)
    {
    }


    template <class Ret>
    static constexpr bool
    canContain()
    {
        return false;
    }
};

template <class Object, class... Others>
template <class Ret>
constexpr bool
ObjectHandleContainer<Object, Others...>::canContain()
{
    if constexpr (std::is_base_of_v<Ret, Object>)
        return true;
    else
        return ObjectHandleContainer<Others...>::template canContain<Ret>();
}

template <class Object, class... Others>
template <class Ret>
inline typename ObjectHandleContainer<Object, Others...>::template GetType<Ret>
ObjectHandleContainer<Object, Others...>::get() const
{
    if constexpr (is_multi_object_handle<Object>::value)
    {
        if constexpr (std::is_same_v<Ret, Object>)
        {
            GetType<Ret> objects;
            for (const auto& obj : object_)
            {
                if (auto ptr = obj.lock())
                    objects.push_back(ptr);
            }
            return objects;
        }
        else
            return others_.template get<Ret>();
    }
    else if constexpr (std::is_base_of_v<Ret, Object>)
        return object_.lock();
    else
        return others_.template get<Ret>();
}

template <class Object, class... Others>
template <class Agg>
inline void
ObjectHandleContainer<Object, Others...>::setFromAggregationIfNotSet(const Agg& agg)
{
    if constexpr (is_multi_object_handle<Object>::value)
    {
        object_ = agg.template getAll<Object>();
    }
    else
    {
        if (!object_.lock())
            object_ = agg.template getOne<Object>();
    }
    others_.setFromAggregationIfNotSet(agg);
}

template <class Object, class... Others>
inline void
ObjectHandleContainer<Object, Others...>::setFromAggregationIfNotSet(const Aggregator& agg)
{
    if constexpr (is_multi_object_handle<Object>::value)
    {
        auto objects = agg.getAll<typename Object::Type>();
        if (objects.size() != object_.size())
        {
            // Number of objects changed, get all objects again
            object_.clear();
            for (auto& obj : objects)
            {
                if (obj)
                    object_.push_back(obj);
            }
        }
    }
    else
    {
        if (!object_.lock())
            object_ = agg.getOne<Object>();
    }
    others_.setFromAggregationIfNotSet(agg);
}


template <class Object, class... Others>
template <class Ret>
inline bool
ObjectHandleContainer<Object, Others...>::isSet() const
{
    if constexpr (std::is_base_of_v<Ret, Object>)
    {
        if constexpr (is_multi_object_handle<Ret>::value)
            return object_.size() > 0;
        else
            return object_.lock() != nullptr;
    }
    else
        return others_.template isSet<Ret>();
}

#endif /* UAVAP_CORE_OBJECT_OBJECTHANDLECONTAINER_HPP_ */
