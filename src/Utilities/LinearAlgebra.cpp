/*
 * LinearAlgebra.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: mircot
 */

#include <iostream>

#include "cpsCore/Utilities/LinearAlgebra.h"

Vector2
rotate2Drad(const Vector2& vec, const FloatingType& rad)
{
	FloatingType c = std::cos(rad);
	FloatingType s = std::sin(rad);
	FloatingType x = vec.x() * c - vec.y() * s;
	FloatingType y = vec.x() * s + vec.y() * c;
	return Vector2(x, y);
}

std::istream&
operator>>(std::istream& is, EigenLine& obj)
{
	Vector3 origin, direction;
	is >> origin;
	is >> direction;
	obj = EigenLine(origin, direction);
	return is;
}

std::istream&
operator>>(std::istream& is, EigenHyperplane& obj)
{
	Vector3 norm, off;
	is >> norm >> off;
	obj.normal();
	obj.offset();
	obj = EigenHyperplane(norm, off);
	return is;
}

FloatingType
boundAngleRad(FloatingType angle)
{
	if (angle > M_PI)
		angle -= 2 * M_PI;
	else if (angle < -M_PI)
		angle += 2 * M_PI;
	return angle;
}

Eigen::Quaternion<FloatingType>
eulerToQuaternion(const Vector3& euler)
{
	FloatingType pitchForQuat = boundAngleRad(euler[1] - M_PI);
	FloatingType yawForQuat = boundAngleRad(-euler[2] - M_PI / 2.0);
	Eigen::Quaternion<FloatingType> attitude(
			AngleAxis(yawForQuat, Vector3::UnitZ())
					* AngleAxis(pitchForQuat, Vector3::UnitY())
					* AngleAxis(euler[0], Vector3::UnitX()));
	attitude.normalize();
	return attitude;
}

Vector3
quaternionToEuler(const Eigen::Quaternion<FloatingType>& quaternion)
{
	Vector3 temp = quaternion.toRotationMatrix().eulerAngles(2, 1, 0);

	FloatingType roll = temp[2];
	FloatingType pitch = temp[1];
	FloatingType yaw = temp[0];
	bool flipPitch = (pitch > M_PI / 2) || (pitch < -M_PI / 2);

	if (flipPitch)
		pitch = boundAngleRad(-pitch - M_PI);
	else
	{
		yaw = boundAngleRad(yaw - M_PI);
		roll = boundAngleRad(roll - M_PI);
	}

	yaw = boundAngleRad(-(yaw - M_PI/2));

	return Vector3(roll, pitch, yaw);
}
