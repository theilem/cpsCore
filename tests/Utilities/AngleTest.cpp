/*
 * AngleTest.cpp
 *
 *  Created on: Oct 11, 2019
 *      Author: mirco
 */
#include <catch2/catch.hpp>

#include "cpsCore/Utilities/Angle.hpp"


TEST_CASE("Test 1")
{

	Angle<float> a(180.0);
	CHECK(a() == Approx(M_PI).epsilon(1e-4));

	a = 90;
	CHECK(a() == Approx(M_PI_2).epsilon(1e-4));

	float rad = a;
	CHECK(rad == Approx(M_PI_2).epsilon(1e-4));

	float deg = a.degrees();
	CHECK(deg == Approx(90.0).epsilon(1e-4));

}