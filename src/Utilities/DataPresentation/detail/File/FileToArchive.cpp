/**
 * @file FileToArchive.cpp
 * @date Nov 13, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */
#include "cpsCore/Utilities/DataPresentation/detail/File/FileToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"

#include <fstream>


FileToArchive::FileToArchive(std::ofstream& file, const ArchiveOptions& opts) :
		options_(opts), file_(file)
{
}

void
FileToArchive::setOptions(const ArchiveOptions& opts)
{
	options_ = opts;
}

void
FileToArchive::append(const char* c, size_t length)
{
	file_.write(c, length);
}

FileToArchive&
FileToArchive::operator<<(const double& doub)
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
