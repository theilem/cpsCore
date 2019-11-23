/**
 * @file FileToArchiveImpl.hpp
 * @date Nov 13, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_FILETOARCHIVEIMPL_HPP_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_FILETOARCHIVEIMPL_HPP_
#include "cpsCore/Utilities/DataPresentation/detail/File/FileToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"

template<typename Type>
FileToArchive&
FileToArchive::operator <<(const Type& cval)
{
	dp::serialize<FileToArchive, Type>(*this, const_cast<Type&>(cval));
	return *this;
}

template<class Type>
inline void
FileToArchive::operator &(const Type& val)
{
	*this << val;
}

template<class Type>
inline FileToArchive&
FileToArchive::operator >>(const Type& val)
{
	return *this;
}

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_FILETOARCHIVEIMPL_HPP_ */
