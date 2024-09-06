//
// Created by mirco on 09.03.21.
//


#include <cpsCore/Utilities/Test/TestInfo.h>
#include "cpsCore/Utilities/Filtering.hpp"

TEST_CASE("Moving Average Filter")
{
	MovingAverageValue<double> value(0.5, 0);

	CHECK(value == 0);
	value = 1.0;
	CHECK(value == 0.5);
	value = 1.0;
	CHECK(value == 0.75);
}

TEST_CASE("Threshold EMA Filter")
{
	ThresholdEMAFilterParams<double> params;
	params.alpha = 0.5;
	params.threshold = 1.0;

	ThresholdEMAValue<double> value(params, 0);
	CHECK(value == 0);
	value = 2.0;
	CHECK(value == 0);
	value = 0.8;
	CHECK(value == 0.4);
	value = 1.2;
	CHECK(value == 0.8);
	value = std::numeric_limits<double>::infinity();
	CHECK(value == 0.8);
	value = std::nan("");
	CHECK(value == 0.8);
	value = 0.5;
	CHECK(value == 0.65);
}