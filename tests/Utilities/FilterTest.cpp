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