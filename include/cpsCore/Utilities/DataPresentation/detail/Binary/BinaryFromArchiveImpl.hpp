/**
 * @file BinaryFromArchiveImpl.hpp
 * @brief Implementation of the template functions in BinaryFromArchive.h
 * @date Nov 14, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BINARYFROMARCHIVEIMPL_HPP_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BINARYFROMARCHIVEIMPL_HPP_

#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"

template<class Type>
BinaryFromArchive&
BinaryFromArchive::operator >>(Type& val)
{
	dp::serialize<BinaryFromArchive, Type>(*this, val);
	return *this;
}

template<class Type>
inline void
BinaryFromArchive::operator &(Type& val)
{
	*this >> val;
}

template<class Type>
inline BinaryFromArchive&
BinaryFromArchive::operator <<(Type& val)
{
	return *this;
}

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BINARYFROMARCHIVEIMPL_HPP_ */
