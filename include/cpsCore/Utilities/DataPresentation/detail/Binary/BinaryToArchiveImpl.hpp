/**
 * @file BinaryToArchiveImpl.hpp
 * @brief Implementation of the template functions in BinaryToArchive.h
 * @date Nov 14, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BINARYTOARCHIVEIMPL_HPP_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BINARYTOARCHIVEIMPL_HPP_

#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"

template<class Type>
BinaryToArchive&
BinaryToArchive::operator <<(const Type& cval)
{
	dp::serialize<BinaryToArchive, Type>(*this, const_cast<Type&>(cval));
	return *this;
}

template<class Type>
inline void
BinaryToArchive::operator &(const Type& val)
{
	*this << val;
}

template<class Type>
inline BinaryToArchive&
BinaryToArchive::operator >>(const Type& val)
{
	return *this;
}

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BINARYTOARCHIVEIMPL_HPP_ */
