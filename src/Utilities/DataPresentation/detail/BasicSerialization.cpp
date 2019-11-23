
/*
 * PODSerialization.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: mircot
 */

#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/File/FileFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/File/FileToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"
#include <cstring>

namespace dp
{


void
load(BinaryFromArchive& ar, char* val, unsigned long bytes)
{
	memcpy(val, ar.begin(), bytes);
	ar.consume(bytes);
}

void
store(BinaryToArchive& ar, char* val, unsigned long bytes)
{
	ar.append(val, bytes);
}

void
load(FileFromArchive& ar, char* val, unsigned long bytes)
{
	ar.read(val, bytes);
}

void
store(FileToArchive& ar, char* val, unsigned long bytes)
{
	ar.append(val, bytes);
}

void
split(BinaryFromArchive& ar, char* val, std::size_t size)
{
	load(ar, val, size);
}

void
split(BinaryToArchive& ar, char* val, std::size_t size)
{
	store(ar, val, size);
}

void
split(FileFromArchive& ar, char* val, std::size_t size)
{
	load(ar, val, size);
}

void
split(FileToArchive& ar, char* val, std::size_t size)
{
	store(ar, val, size);
}

}//namespace dp