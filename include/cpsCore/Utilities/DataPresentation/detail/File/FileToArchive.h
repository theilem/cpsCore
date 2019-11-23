/**
 * @file FileToArchive.h
 * @date Nov 13, 2018
 * @author Mirco Theile, mirco.theile@tum.de
 * @brief
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_FILETOARCHIVE_H_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_FILETOARCHIVE_H_

#include "cpsCore/Utilities/DataPresentation/detail/ArchiveOptions.h"

class FileToArchive
{
public:
	/**
	 * @brief Constructor wrapping around a string
	 * @param str String to be wrapped around and filled with serialization
	 */
	FileToArchive(std::ofstream& str, const ArchiveOptions& opts = ArchiveOptions());

	void
	setOptions(const ArchiveOptions& opts);

	/**
	 * @brief Append the string with length characters from c
	 * @param c Characters to be appended
	 * @param length Number of characters to be appended
	 */
	void
	append(const char* c, size_t length);

	template<typename Type>
	FileToArchive&
	operator<<(const Type& cval);

	/**
	 * @brief Handle double according to setting compressDouble_
	 * @param doub
	 * @return
	 */
	FileToArchive&
	operator<<(const double& doub);

	/**
	 * @brief Operator & for flush in
	 * @param val Data to be flushed in
	 */
	template<class Type>
	void
	operator&(const Type& val);

	/**
	 * @brief Flush out operator. Does nothing
	 * @param val Data to be flushed out.
	 * @return The archive itself
	 */
	template<class Type>
	FileToArchive&
	operator>>(const Type& val);

private:

	ArchiveOptions options_;

	std::ofstream& file_;
};

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_FILETOARCHIVE_H_ */
