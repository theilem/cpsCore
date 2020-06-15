/**
 * @file BasicSerialization.h
 * @brief Definition of basic serialization methods.
 *
 * The implementation of the basic methods can be found in BasicSerializationImpl.hpp
 * @date Aug 22, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_BASICSERIALIZATION_H_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_BASICSERIALIZATION_H_

#include "cpsCore/Utilities/Optional.hpp"
#include "cpsCore/Configuration/Parameter.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/SerializeCustom.h"
#include "cpsCore/Utilities/DataPresentation/detail/Split.h"

#include <map>
#include <vector>
#include <Eigen/Dense>

class BinaryFromArchive;
class BinaryToArchive;
class FileFromArchive;
class FileToArchive;

namespace dp
{

/**
 * @brief General method to load a number of characters from a BinaryFromArchive
 * @param ar Archive containing the characters to be loaded
 * @param val Extracted characters
 * @param bytes Number of characters to be extracted
 */
void
load(BinaryFromArchive& ar, char* val, unsigned long bytes);

/**
 * @brief General method to store a number of characters to a BinaryToArchive
 * @param ar Archive into which the characters will be added
 * @param val Ptr to characters to be added
 * @param bytes Number of characters
 */
void
store(BinaryToArchive& ar, char* val, unsigned long bytes);

/**
 * @brief General method to load a number of characters from a FileFromArchive
 * @param ar Archive containing the characters to be loaded
 * @param val Extracted characters
 * @param bytes Number of characters to be extracted
 */
void
load(FileFromArchive& ar, char* val, unsigned long bytes);

/**
 * @brief General method to store a number of characters to a FileToArchive
 * @param ar Archive into which the characters will be added
 * @param val Ptr to characters to be added
 * @param bytes Number of characters
 */
void
store(FileToArchive& ar, char* val, unsigned long bytes);
//
///**
// * @brief General split functionality for loading
// * @param ar From archive
// * @param val any value to be loaded differently than it is stored
// */
//template<class Type>
//void
//split(BinaryFromArchive& ar, Type& val);
//
///**
// * @brief General split functionality for storing
// * @param ar To archive
// * @param val any value to be stored differently than it is loaded
// */
//template<class Type>
//void
//split(BinaryToArchive& ar, Type& val);
//
///**
// * @brief General split functionality for loading
// * @param ar From archive
// * @param val any value to be loaded differently than it is stored
// */
//template<class Type>
//void
//split(FileFromArchive& ar, Type& val);
//
///**
// * @brief General split functionality for storing
// * @param ar To archive
// * @param val any value to be stored differently than it is loaded
// */
//template<class Type>
//void
//split(FileToArchive& ar, Type& val);
//
//void
//split(BinaryFromArchive& ar, char* val, std::size_t size);
//
//void
//split(BinaryToArchive& ar, char* val, std::size_t size);
//
//void
//split(FileFromArchive& ar, char* val, std::size_t size);
//
//void
//split(FileToArchive& ar, char* val, std::size_t size);

template<class Archive>
void
serialize(Archive& ar, char* val, std::size_t size);

/**
 * @brief Load function for POD types that are not enum or SerializeCustom objects.
 */
template<class Archive, typename PODType>
void
load(Archive& ar,
		typename std::enable_if<
				std::is_pod<PODType>::value && !std::is_enum<PODType>::value
						&& !std::is_base_of<SerializeCustom, PODType>::value, PODType>::type& val);

/**
 * @brief Store function for POD types that are not enum or SerializeCustom objects.
 */
template<class Archive, typename PODType>
void
store(Archive& ar,
		typename std::enable_if<
				std::is_pod<PODType>::value && !std::is_enum<PODType>::value
						&& !std::is_base_of<SerializeCustom, PODType>::value, PODType>::type& val);

/**
 * @brief Serialize function for POD types that are not enum or SerializeCustom objects.
 */
template<class Archive, typename PODType>
inline void
serialize(Archive& ar,
		typename std::enable_if<
				std::is_pod<PODType>::value && !std::is_enum<PODType>::value
						&& !std::is_base_of<SerializeCustom, PODType>::value, PODType>::type& val);

/**
 * @brief Load function for all enum data types
 */
template<class Archive, typename EnumType>
void
load(Archive& ar, typename std::enable_if<std::is_enum<EnumType>::value, EnumType>::type& val);

/**
 * @brief store function for all enum data types
 */
template<class Archive, typename EnumType>
void
store(Archive& ar, typename std::enable_if<std::is_enum<EnumType>::value, EnumType>::type& val);

/**
 * @brief Serialize function for all enum data types
 */
template<class Archive, class EnumType>
inline void
serialize(Archive& ar, typename std::enable_if<std::is_enum<EnumType>::value, EnumType>::type& val);

/**
 * @brief is_vector struct false_type because T is not a vector
 */
template<typename T>
struct is_vector: public std::false_type
{
};

/**
 * @brief is_vector struct true_type because T is a vector
 */
template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type
{
};

/**
 * @brief Load function for any vector
 */
template<class Archive, typename Type>
void
load(Archive& ar, typename std::enable_if<is_vector<Type>::value, Type>::type& val);

/**
 * @brief Store function for any vector
 */
template<class Archive, typename Type>
void
store(Archive& ar, typename std::enable_if<is_vector<Type>::value, Type>::type& val);

/**
 * @brief Serialize function for any vector
 */
template<class Archive, typename Type>
inline void
serialize(Archive& ar, typename std::enable_if<is_vector<Type>::value, Type>::type& val);

/**
 * @brief is_map struct false_type because T is not a map
 */
template<typename T>
struct is_map: public std::false_type
{
};

/**
 * @brief is_map struct true_type because T is a map
 */
template<typename K, typename T, typename C, typename A>
struct is_map<std::map<K, T, C, A>> : public std::true_type
{
};

/**
 * @brief Load function for any map
 */
template<class Archive, typename Type>
void
load(Archive& ar, typename std::enable_if<is_map<Type>::value, Type>::type& val);

/**
 * @brief Store function for any map
 */
template<class Archive, typename Type>
void
store(Archive& ar, typename std::enable_if<is_map<Type>::value, Type>::type& val);

/**
 * @brief Serialize function for any map
 */
template<class Archive, typename Type>
inline void
serialize(Archive& ar, typename std::enable_if<is_map<Type>::value, Type>::type& val);

/**
 * @brief Load function for strings
 */
template<class Archive, typename Type>
void
load(Archive& ar, typename std::enable_if<std::is_same<Type, std::string>::value, Type>::type& val);

/**
 * @brief Store function for strings
 */
template<class Archive, typename Type>
void
store(Archive& ar,
		typename std::enable_if<std::is_same<Type, std::string>::value, Type>::type& val);

/**
 * @brief Serialize function for strings
 */
template<class Archive, typename Type>
inline void
serialize(Archive& ar,
		typename std::enable_if<std::is_same<Type, std::string>::value, Type>::type& val);
//
///**
// * @brief is_optional struct false_type because T is not a boost optional
// */
//template<typename T>
//struct is_optional: public std::false_type
//{
//};
//#ifndef ERIKA
///**
// * @brief is_optional struct true_type because T is a boost optional
// */
//template<typename T>
//struct is_optional<boost::optional<T>> : public std::true_type
//{
//};
//#endif
//
///**
// * @brief Load function for boost optionals
// */
//template<class Archive, typename Type>
//void
//load(Archive& ar, typename std::enable_if<is_optional<Type>::value, Type>::type& val);
//
///**
// * @brief Store function for boost optionals
// */
//template<class Archive, typename Type>
//void
//store(Archive& ar, typename std::enable_if<is_optional<Type>::value, Type>::type& val);
//
///**
// * @brief Serialize function for boost optionals
// */
//template<class Archive, typename Type>
//inline void
//serialize(Archive& ar, typename std::enable_if<is_optional<Type>::value, Type>::type& val);


template<typename T>
struct isOptional: public std::false_type
{
};
/**
 * @brief is_optional struct true_type because T is a boost optional
 */
template<typename T>
struct isOptional<Optional<T>> : public std::true_type
{
};

/**
 * @brief Load function for boost optionals
 */
template<class Archive, typename Type>
void
load(Archive& ar, typename std::enable_if<isOptional<Type>::value, Type>::type& val);

/**
 * @brief Store function for boost optionals
 */
template<class Archive, typename Type>
void
store(Archive& ar, typename std::enable_if<isOptional<Type>::value, Type>::type& val);

/**
 * @brief Serialize function for boost optionals
 */
template<class Archive, typename Type>
void
serialize(Archive& ar, typename std::enable_if<isOptional<Type>::value, Type>::type& val);

/**
 * @brief is_pair struct false_type because T is not a pair
 */
template<typename T>
struct is_pair: public std::false_type
{
};

/**
 * @brief is_pair struct true_type because T is a pair
 */
template<typename T, typename A>
struct is_pair<std::pair<T, A>> : public std::true_type
{
};

/**
 * @brief Serialize function for any pair
 */
template<class Archive, typename Type>
void
serialize(Archive& ar, typename std::enable_if<is_pair<Type>::value, Type>::type& val);

template<class Archive, typename Type>
void
serialize(Archive& ar, typename std::enable_if<is_parameter<Type>::value, Type>::type& val);

template<class Archive, typename Type>
void
serialize(Archive& ar, typename std::enable_if<is_parameter_set<Type>::value, Type>::type& val);


/**
 * Eigen Matrix serialization operations
 */

template<typename T>
struct is_eigen_mat: public std::false_type
{
};

template<typename T>
struct is_eigen_mat<Eigen::Matrix<T, -1, -1>> : public std::true_type
{
};

/**
 * @brief Load function for any Matrix
 */
template<class Archive, typename Type>
void
load(Archive& ar, std::enable_if_t<is_eigen_mat<Type>::value, Type>& val);

/**
 * @brief Store function for any Matrix
 */
template<class Archive, typename Type>
void
store(Archive& ar, std::enable_if_t<is_eigen_mat<Type>::value, Type>& val);

/**
 * @brief Serialize function for any Matrix
 */
template<class Archive, typename Type>
inline void
serialize(Archive& ar, std::enable_if_t<is_eigen_mat<Type>::value, Type>& val);





} /* dp */

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_BASICSERIALIZATION_H_ */
