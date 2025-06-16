/*
 * BinaryOutArchive.cpp
 *
 *  Created on: Aug 24, 2017
 *      Author: mircot
 */
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"

BinaryToArchive::BinaryToArchive(std::string& str, const ArchiveOptions& opts) :
options_(opts), string_(str)
{
}

void
BinaryToArchive::append(const char* c, size_t length)
{
	string_.append(c, length);
}

void
BinaryToArchive::setOptions(const ArchiveOptions& opts)
{
	options_ = opts;
}

BinaryToArchive&
BinaryToArchive::operator <<(const double& doub)
{
	if (options_.compressDouble())
	{
		float flo = static_cast<float>(doub);
		dp::store(*this, reinterpret_cast<char*>(&flo), sizeof(float));
	}
	else
	{
		dp::store(*this, reinterpret_cast<char*>(&const_cast<double&>(doub)), sizeof(double));
	}
	return *this;
}

bool
BinaryToArchive::compressDouble() const
{
	return options_.compressDouble();
}
