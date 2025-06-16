/*
 * BinaryInArchive.cpp
 *
 *  Created on: Aug 24, 2017
 *      Author: mircot
 */
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"
#include <cstring>

BinaryFromArchive::BinaryFromArchive(const std::string& str, const ArchiveOptions& opts) :
		options_(opts), string_(str), idx_(0)
{
	static_assert(sizeof(double) == 8, "Double precision is not 8Byte");
}

const char*
BinaryFromArchive::begin()
{
	if (idx_ >= string_.size())
		throw ArchiveError("Archive idx too large.");
	return &string_[idx_];
}

void
BinaryFromArchive::consume(unsigned long bytes)
{
	idx_ += bytes;
}

std::string
BinaryFromArchive::getRemaining()
{
	return string_.substr(idx_, string_.size() - 1);
}

std::size_t
BinaryFromArchive::getConsumed()
{
	return idx_;
}

bool
BinaryFromArchive::compressDouble() const
{
	return options_.compressDouble();
}

void
BinaryFromArchive::setOptions(const ArchiveOptions& opts)
{
	options_ = opts;
}

BinaryFromArchive&
BinaryFromArchive::operator >>(double& doub)
{
	if (options_.compressDouble())
	{
		float flo;
		dp::load(*this, reinterpret_cast<char*>(&flo), sizeof(float));
		doub = static_cast<double>(flo);
	}
	else
	{
		dp::load(*this, reinterpret_cast<char*>(&doub), sizeof(double));
	}
	return *this;
}

void
BinaryFromArchive::read(char* val, unsigned long bytes)
{
	memcpy(static_cast<void*>(val), string_.c_str() + idx_, bytes);
	consume(bytes);
}
