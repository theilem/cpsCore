/**
 * @file BinaryToArchive.h
 * @brief Defines the BinaryRoArchive
 * @date Aug 22, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_BINARYTOARCHIVE_H_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_BINARYTOARCHIVE_H_
#include <string>

#include "cpsCore/Utilities/DataPresentation/detail/ArchiveOptions.h"

/**
 * @brief Archive wrapper around a string to serialize data
 */
class BinaryToArchive
{
public:

	/**
	 * @brief Constructor wrapping around a string
	 * @param str String to be wrapped around and filled with serialization
	 */
	BinaryToArchive(std::string& str, const ArchiveOptions& opts = ArchiveOptions());

	void
	setOptions(const ArchiveOptions& opts);

	/**
	 * @brief Append the string with length characters from c
	 * @param c Characters to be appended
	 * @param length Number of characters to be appended
	 */
	void
	append(const char* c, size_t length);

	/**
	 * @brief Flush in operator for non protobuf objects.
	 *
	 * Serializes the object that is not a protobuf object.
	 * Uses serialization from BasicSerialization.h and CustomSerialization.h
	 * @param cval Data to be serialized
	 * @return The archive itself
	 */
	template<typename Type>
	BinaryToArchive&
	operator <<(const Type& cval);

	/**
	 * @brief Handle double according to setting compressDouble_
	 * @param doub
	 * @return
	 */
	BinaryToArchive&
	operator <<(const double& doub);

	/**
	 * @brief Operator & for flush in
	 * @param val Data to be flushed in
	 */
	template<class Type>
	void
	operator &(const Type& val);

	/**
	 * @brief Flush out operator. Does nothing
	 * @param val Data to be flushed out.
	 * @return The archive itself
	 */
	template<class Type>
	BinaryToArchive&
	operator >>(const Type& val);


	bool
	compressDouble() const;

private:

	ArchiveOptions options_;

	std::string& string_; //!< String that is wrapped around and filled with serialized data.
};

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_BINARYTOARCHIVE_H_ */
