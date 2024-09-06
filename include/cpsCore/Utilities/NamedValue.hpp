/*
 * Parameter.h
 *
 *  Created on: Jun 24, 2019
 *      Author: mirco
 */

#ifndef CPSCORE_UTILITIES_NAMEDVALUE_HPP_
#define CPSCORE_UTILITIES_NAMEDVALUE_HPP_

#include <string>
#include <cpsCore/Utilities/LinearAlgebra.h>

template <typename Type>
struct NamedValue
{
    using ValueType = Type;

    NamedValue<Type>&
    operator=(const NamedValue<Type>& other)
    {
        value = other.value;
        return *this;
    }

    NamedValue<Type>&
    operator=(const Type& val)
    {
        value = val;
        return *this;
    }

    inline const Type&
    operator()() const
    {
        return value;
    }

    inline Type&
    operator()()
    {
        return value;
    }

    inline void
    setValue(const Type& val)
    {
        value = val;
    }

    Type value;
    const std::string id;
};

template <typename Type>
class NamedValueParser
{
public:
    NamedValueParser() = default;

    template <typename NamedValueSet>
    const std::map<std::string, Type>&
    parse(const NamedValueSet& set)
    {
        set.parse(*this);
        return values_;
    }

    template <typename NamedValueType>
    void
    operator&(const NamedValueType& value)
    {
        if constexpr (std::is_convertible_v<typename NamedValueType::ValueType, Type>)
            values_[value.id] = static_cast<Type>(value.value);
        else if constexpr (is_eigen_vector<typename NamedValueType::ValueType>::value)
        {
            values_[value.id + "_x"] = static_cast<Type>(value.value.x());
            values_[value.id + "_y"] = static_cast<Type>(value.value.y());
            values_[value.id + "_z"] = static_cast<Type>(value.value.z());
        }
    }

private:
    std::map<std::string, Type> values_;
};

class NamedValuePrinter
{
public:
    explicit
    NamedValuePrinter(std::ostream& os) :
        os_(os)
    {
    }

    template <typename Type>
    NamedValuePrinter&
    operator&(const NamedValue<Type>& value)
    {
        os_ << value.id << ": " << value.value << std::endl;
        return *this;
    }

    template <typename NamedValueSet>
    std::ostream&
    operator<<(const NamedValueSet& set)
    {
        return print(set);
    }

    template <typename NamedValueSet>
    std::ostream&
    print(const NamedValueSet& set)
    {
        os_ << set.name << std::endl;
        set.parse(*this);
        return os_;
    }

private:
    std::ostream& os_;
};


template<typename, typename = void>
struct is_named_value_set : std::false_type {};

template<typename T>
struct is_named_value_set<T, std::void_t<decltype(std::declval<T>().parse(std::declval<int&>()))>> : std::true_type {};


template <typename Type>
struct is_named_value : public std::false_type
{
};

template <typename Type>
struct is_named_value<NamedValue<Type>> : public std::true_type
{
};


template <typename Type>
std::ostream& operator<<(std::ostream& os, const std::enable_if_t<is_named_value_set<Type>::value, Type>& value)
{
    NamedValuePrinter printer(os);
    printer << value;
    return os;
}


#endif /* CPSCORE_UTILITIES_NAMEDVALUE_HPP_ */
