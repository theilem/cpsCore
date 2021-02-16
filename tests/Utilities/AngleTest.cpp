/*
 * AngleTest.cpp
 *
 *  Created on: Oct 11, 2019
 *      Author: mirco
 */
#include <cpsCore/Utilities/Test/TestInfo.h>
#include <cpsCore/Utilities/Angle.hpp>
#include <cpsCore/Utilities/DataPresentation/BinarySerialization.hpp>


TEST_CASE("Angle Test")
{
	Angle<float> a(180.0);
	CHECK(a() == Approx(M_PI).margin(1e-4));

	a = 90;
	CHECK(a() == Approx(M_PI_2).margin(1e-4));

	float rad = a;
	CHECK(rad == Approx(M_PI_2).margin(1e-4));

	float deg = a.degrees();
	CHECK(deg == Approx(90.0).margin(1e-4));
}

TEST_CASE("Angle Test DP")
{
	Angle<float> a(90.0);
	CHECK(a() == Approx(M_PI_2).margin(1e-4));
	float deg = a.degrees();
	CHECK(deg == Approx(90.0).margin(1e-4));

	auto p = dp::serialize(a);
	auto desAngle = dp::deserialize<Angle<float>>(p);
	CHECK(desAngle() == Approx(M_PI_2).margin(1e-4));
	CHECK(desAngle.degrees() == Approx(90.0).margin(1e-4));
}

TEST_CASE("Angle Test static cast test")
{
	Angle<float> a(90.0);
	CHECK(M_PI_2 == Approx((float)a).margin(1e-6));
}