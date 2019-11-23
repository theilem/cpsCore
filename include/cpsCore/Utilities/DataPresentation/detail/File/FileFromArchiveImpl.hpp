/**
 * @file FileFromArchiveImpl.hpp
 * @date Nov 13, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_FILEFROMARCHIVEIMPL_HPP_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_FILEFROMARCHIVEIMPL_HPP_

#include "cpsCore/Utilities/DataPresentation/detail/File/FileFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"

template<class Type>
FileFromArchive&
FileFromArchive::operator>>(Type& val)
{
	dp::serialize<FileFromArchive, Type>(*this, val);
	return *this;
}

template<class Type>
void
FileFromArchive::operator&(Type& val)
{
	*this >> val;
}

template<class Type>
FileFromArchive&
FileFromArchive::operator<<(Type& val)
{
	return *this;
}

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_FILEFROMARCHIVEIMPL_HPP_ */
