//
// Created by Mirco Theile on 7/6/25.
//

#ifndef MULTIOBJECTHANDLE_H
#define MULTIOBJECTHANDLE_H
#include <type_traits>
#include <vector>

template<typename Object>
struct MultiObjectHandle: std::vector<std::weak_ptr<Object> >
{
    using Type = Object;
};

template <typename Object>
using Multi = MultiObjectHandle<Object>;

template <typename>
struct is_multi_object_handle : std::false_type
{
};

template <typename Object>
struct is_multi_object_handle<MultiObjectHandle<Object>> : std::true_type
{
};

#endif //MULTIOBJECTHANDLE_H
