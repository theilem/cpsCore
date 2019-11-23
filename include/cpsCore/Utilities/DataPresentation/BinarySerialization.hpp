/*
 * BinarySerialization.hpp
 *
 *  Created on: Jul 27, 2018
 *      Author: mircot
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_BINARYSERIALIZATION_HPP_
#define UAVAP_CORE_DATAPRESENTATION_BINARYSERIALIZATION_HPP_

#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"
#include "cpsCore/Utilities/DataPresentation/detail/File/FileFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/File/FileToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/File/FileFromArchiveImpl.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/File/FileToArchiveImpl.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchiveImpl.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchiveImpl.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerializationImpl.hpp"
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Logging/CPSLogger.h"

namespace dp
{

template<typename T>
inline Packet
serialize(const T& data, const ArchiveOptions& options = ArchiveOptions())
{
	std::string str;
	BinaryToArchive archive(str, options);
	archive << data;
	return Packet(str);
}

template<typename T>
inline void
serialize(const T& data, std::ofstream& file, const ArchiveOptions& options = ArchiveOptions())
{
	FileToArchive archive(file, options);
	archive << data;
}

template<typename T>
inline T
deserialize(const Packet& packet, const ArchiveOptions& options = ArchiveOptions())
{
	if (packet.getSize() == 0)
	{
		CPSLOG_WARN << "packet empty";
		return T();
	}
	BinaryFromArchive archive(packet.getBuffer(), options);
	try
	{
		T t;
		archive >> t;
		return t;
	} catch (ArchiveError& err)
	{
		CPSLOG_WARN << "Invalid Packet. Error: " << err.what();
		return T();
	}
}

template<typename T>
inline T
deserialize(std::ifstream& file, const ArchiveOptions& options = ArchiveOptions())
{
	FileFromArchive archive(file, options);
	try
	{
		T t;
		archive >> t;
		return t;
	} catch (ArchiveError& err)
	{
		CPSLOG_WARN << "Invalid Packet. Error: " << err.what();
		return T();
	}
}

template<typename T>
inline T
extract(Packet& packet, const ArchiveOptions& options = ArchiveOptions())
{
	if (packet.getSize() == 0)
	{
		return T();
	}
	BinaryFromArchive archive(packet.getBuffer(), options);
	try
	{
		T t;
		archive >> t;
		packet.consume(archive.getConsumed());
		return t;
	} catch (ArchiveError& err)
	{
		CPSLOG_WARN << "Invalid Packet. Error: " << err.what();
		return T();
	}
}

} //namespace dp

#endif /* UAVAP_CORE_DATAPRESENTATION_BINARYSERIALIZATION_HPP_ */
