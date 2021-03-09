//
// Created by mirco on 09.03.21.
//

#ifndef CPSCORE_FILTERING_HPP
#define CPSCORE_FILTERING_HPP

template<typename Type>
class IFilteredValue
{
public:

	explicit
	IFilteredValue(const Type& val) : value_(val)
	{}

	operator Type() const
	{
		return value_;
	}

	const Type&
	operator()() const
	{
		return value_;
	}

	IFilteredValue<Type>&
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

template<typename Type>
class MovingAverageValue : public IFilteredValue<Type>
{
public:

	MovingAverageValue(const Type& alpha, const Type& init) : IFilteredValue<Type>(init), alpha_(alpha)
	{}

	MovingAverageValue<Type>&
	operator=(const Type& val)
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


#endif //CPSCORE_FILTERING_HPP
