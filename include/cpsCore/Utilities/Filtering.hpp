//
// Created by mirco on 09.03.21.
//

#ifndef CPSCORE_FILTERING_HPP
#define CPSCORE_FILTERING_HPP

#include <cpsCore/Utilities/LinearAlgebra.h>
#include <cpsCore/Configuration/Parameter.hpp>

template <typename Type>
class IFilteredValue
{
public:
    explicit
    IFilteredValue(const Type& val) : value_(val)
    {
    }

    virtual ~IFilteredValue() = default;

    operator Type() const
    {
        return value_;
    }

    const Type&
    operator()() const
    {
        return value_;
    }

    virtual IFilteredValue<Type>&
    operator=(const Type& val)
    {
        applyFilter(val);
        return *this;
    }

protected:
    virtual const Type&
    applyFilter(const Type& val) = 0;

    Type value_;
};

template <typename Type>
class MovingAverageValue : public IFilteredValue<Type>
{
public:
    MovingAverageValue(const Type& alpha, const Type& init) : IFilteredValue<Type>(init), alpha_(alpha)
    {
    }

    MovingAverageValue<Type>&
    operator=(const Type& val) override
    {
        this->applyFilter(val);
        return *this;
    }

    Type&
    getAlpha()
    {
        return alpha_;
    }

protected:
    const Type&
    applyFilter(const Type& val) override
    {
        this->value_ = alpha_ * val + (1 - alpha_) * this->value_;
        return this->value_;
    }

private:
    Type alpha_{1.0};
};

template <typename Type>
struct ThresholdEMAFilterParams
{
    Parameter<Type> alpha = {0.1, "alpha", true};
    Parameter<Type> threshold = {std::numeric_limits<FloatingType>::max(), "threshold", true};
};

template <typename Type>
class ThresholdEMAValue : public IFilteredValue<Type>
{
public:
    ThresholdEMAValue(const ThresholdEMAFilterParams<Type>& params, const FloatingType& init) :
        IFilteredValue<Type>(init),
        params_(params)
    {
    }

    ThresholdEMAValue<Type>&
    operator=(const Type& val) override
    {
        this->applyFilter(val);
        return *this;
    }

private:
    const Type&
    applyFilter(const Type& val) override
    {
        if (std::fabs(val - this->value_) < params_.threshold() && std::isfinite(val))
        {
            this->value_ = params_.alpha() * val + (1 - params_.alpha()) * this->value_;
        }
        return this->value_;
    }

    const ThresholdEMAFilterParams<Type>& params_;
};


#endif //CPSCORE_FILTERING_HPP
