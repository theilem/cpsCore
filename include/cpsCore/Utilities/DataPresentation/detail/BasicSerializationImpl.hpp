/**
 * @file BasicSerializationImpl.hpp
 * @brief Implementation of the generic serialization methods in BasicSerialization.h
 * @date Nov 14, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BASICSERIALIZATIONIMPL_HPP_
#define UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BASICSERIALIZATIONIMPL_HPP_

#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"
#include <cpsCore/Configuration/Configuration.hpp>

template <class Type>
inline void
dp::split(BinaryFromArchive& ar, Type& val)
{
    load<BinaryFromArchive, Type>(ar, val);
}

template <class Type>
inline void
dp::split(BinaryToArchive& ar, Type& val)
{
    store<BinaryToArchive, Type>(ar, val);
}

template <class Type>
inline void
dp::split(FileFromArchive& ar, Type& val)
{
    load<FileFromArchive, Type>(ar, val);
}

template <class Type>
inline void
dp::split(FileToArchive& ar, Type& val)
{
    store<FileToArchive, Type>(ar, val);
}

template <class Archive>
inline void
dp::serialize(Archive& ar, char* val, std::size_t size)
{
    split(ar, val, size);
}

template <class Archive, typename EnumType>
void
dp::load(Archive& ar, typename std::enable_if<std::is_enum<EnumType>::value, EnumType>::type& val)
{
    uint8_t e;
    ar >> e;
    val = static_cast<EnumType>(e);
}

template <class Archive, typename EnumType>
void
dp::store(Archive& ar, typename std::enable_if<std::is_enum<EnumType>::value, EnumType>::type& val)
{
    ar << static_cast<uint8_t>(val);
}

template <class Archive, class EnumType>
void
dp::serialize(Archive& ar,
              typename std::enable_if<std::is_enum<EnumType>::value, EnumType>::type& val)
{
    split(ar, val);
}

template <class Archive, typename PODType>
inline void
dp::load(Archive& ar,
         typename std::enable_if<
             std::is_pod<PODType>::value && !std::is_enum<PODType>::value
             && !std::is_base_of<SerializeCustom, PODType>::value, PODType>::type& val)
{
    load(ar, reinterpret_cast<char*>(&val), sizeof(PODType));
}

template <class Archive, typename PODType>
inline void
dp::store(Archive& ar,
          typename std::enable_if<
              std::is_pod<PODType>::value && !std::is_enum<PODType>::value
              && !std::is_base_of<SerializeCustom, PODType>::value, PODType>::type& val)
{
    store(ar, reinterpret_cast<char*>(&val), sizeof(PODType));
}

template <class Archive, typename PODType>
inline void
dp::serialize(Archive& ar,
              typename std::enable_if<
                  std::is_pod<PODType>::value && !std::is_enum<PODType>::value
                  && !std::is_base_of<SerializeCustom, PODType>::value, PODType>::type& val)
{
    split(ar, val);
}

template <class Archive, typename Type>
inline void
dp::load(Archive& ar, typename std::enable_if<is_vector<Type>::value, Type>::type& val)
{
    std::uint16_t size;
    ar >> size;
    for (std::uint16_t i = 0; i < size; ++i)
    {
        typename Type::value_type tmp;
        ar >> tmp;
        val.push_back(tmp);
    }
}

template <class Archive, typename Type>
inline void
dp::store(Archive& ar, typename std::enable_if<is_vector<Type>::value, Type>::type& val)
{
    ar << static_cast<std::uint16_t>(val.size());
    for (auto& it : val)
    {
        ar << it;
    }
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, typename std::enable_if<is_vector<Type>::value, Type>::type& val)
{
    split(ar, val);
}

//template<class Archive, typename Type>
//inline void
//dp::load(Archive& ar, typename std::enable_if<is_optional<Type>::value, Type>::type& val)
//{
//	bool init;
//	ar >> init;
//	if (init)
//	{
//		Type temp;
//		ar >> temp;
//		val = temp;
//	}
//}
//
//template<class Archive, typename Type>
//inline void
//dp::store(Archive& ar, typename std::enable_if<is_optional<Type>::value, Type>::type& val)
//{
//	ar << val.is_initialized();
//	if (val)
//	{
//		ar << *val;
//	}
//}
//
//template<class Archive, typename Type>
//inline void
//dp::serialize(Archive& ar, typename std::enable_if<is_optional<Type>::value, Type>::type& val)
//{
//	split(ar, val);
//}

template <class Archive, typename Type>
inline void
dp::load(Archive& ar, typename std::enable_if<isOptional<Type>::value, Type>::type& val)
{
    bool init;
    ar >> init;
    if (init)
    {
        typename Type::value_type temp;
        ar >> temp;
        val = temp;
    }
}

template <class Archive, typename Type>
inline void
dp::store(Archive& ar, typename std::enable_if<isOptional<Type>::value, Type>::type& val)
{
    ar << static_cast<bool>(val.has_value());
    if (val.has_value())
    {
        ar << *val;
    }
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, typename std::enable_if<isOptional<Type>::value, Type>::type& val)
{
    split(ar, val);
}

template <class Archive, typename Type>
void
dp::load(Archive& ar, typename std::enable_if<is_array<Type>::value && !std::is_pod_v<Type>, Type>::type& val)
{
    for (std::size_t i = 0; i < val.size(); ++i)
    {
        typename Type::value_type t;
        ar >> t;
        val[i] = t;
    }
}

template <class Archive, typename Type>
void
dp::store(Archive& ar, typename std::enable_if<is_array<Type>::value && !std::is_pod_v<Type>, Type>::type& val)
{
    for (const auto& v: val)
    {
        ar << v;
    }
}

template <class Archive, typename Type>
void
dp::serialize(Archive& ar, typename std::enable_if<is_array<Type>::value && !std::is_pod_v<Type>, Type>::type& val)
{
    split(ar, val);
}

template <class Archive, typename Type>
inline void
dp::load(Archive& ar, typename std::enable_if<is_map<Type>::value, Type>::type& val)
{
    val.clear();
    uint8_t size;
    ar >> size;
    for (uint8_t i = 0; i < size; ++i)
    {
        typename Type::mapped_type valTmp;
        typename Type::key_type keyTmp;
        ar >> keyTmp;
        ar >> valTmp;
        val.insert(std::make_pair(keyTmp, valTmp));
    }
}

template <class Archive, typename Type>
inline void
dp::store(Archive& ar, typename std::enable_if<is_map<Type>::value, Type>::type& val)
{
    ar << static_cast<uint8_t>(val.size());
    for (auto& it : val)
    {
        ar << it.first;
        ar << it.second;
    }
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, typename std::enable_if<is_map<Type>::value, Type>::type& val)
{
    split(ar, val);
}

template <class Archive, typename Type>
inline void
dp::load(Archive& ar,
         typename std::enable_if<std::is_same<Type, std::string>::value, Type>::type& val)
{
    std::uint16_t size;
    ar >> size;
    val.resize(size);
    ar.read(const_cast<char*>(val.data()), size);
}

template <class Archive, typename Type>
inline void
dp::store(Archive& ar,
          typename std::enable_if<std::is_same<Type, std::string>::value, Type>::type& val)
{
    ar << static_cast<std::uint16_t>(val.size());
    ar.append(val.c_str(), val.size());
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar,
              typename std::enable_if<std::is_same<Type, std::string>::value, Type>::type& val)
{
    split(ar, val);
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, typename std::enable_if<is_pair<Type>::value, Type>::type& val)
{
    ar & val.first;
    ar & val.second;
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, typename std::enable_if<is_parameter<Type>::value, Type>::type& val)
{
    ar & val.value;
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, typename std::enable_if<is_parameter_set<Type>::value, Type>::type& val)
{
    val.configure(ar);
}

template <class Archive, typename Type>
void
dp::load(Archive& ar, std::enable_if_t<is_eigen_mat<Type>::value, Type>& val)
{
    Eigen::Index rows(Type::RowsAtCompileTime);
    Eigen::Index cols(Type::ColsAtCompileTime);
    if constexpr (Type::RowsAtCompileTime == Eigen::Dynamic)
    {
        // If the number of rows is dynamic, we need to load the size
        ar >> rows;
    }
    if constexpr (Type::ColsAtCompileTime == Eigen::Dynamic)
    {
        // If the number of columns is dynamic, we need to load the size
        ar >> cols;
    }
    if constexpr (std::is_same_v<typename Type::Base::Scalar, double>)
    {
        if (ar.compressDouble())
        {
            // If the matrix is of type double and compression is enabled, read as float
            Eigen::Matrix<float, Type::RowsAtCompileTime, Type::ColsAtCompileTime> temp = Eigen::Matrix<float, Type::RowsAtCompileTime, Type::ColsAtCompileTime>::Zero(rows, cols);
            ar.read(reinterpret_cast<char*>(temp.data()), temp.size() * sizeof(float));
            val = temp.template cast<double>();
            return;
        }
    }
    // Resize the matrix to the loaded size
    val.resize(rows, cols);

    ar.read(reinterpret_cast<char*>(val.data()), val.size() * sizeof(typename Type::Base::Scalar));
}

template <class Archive, typename Type>
void
dp::store(Archive& ar, std::enable_if_t<is_eigen_mat<Type>::value, Type>& val)
{
    if constexpr (Type::RowsAtCompileTime == Eigen::Dynamic)
    {
        // If the number of rows is dynamic, we need to store the size
        ar << static_cast<Eigen::Index>(val.rows());
    }
    if constexpr (Type::ColsAtCompileTime == Eigen::Dynamic)
    {
        // If the number of columns is dynamic, we need to store the size
        ar << static_cast<Eigen::Index>(val.cols());
    }
    if constexpr (std::is_same_v<typename Type::Base::Scalar, double>)
    {
        if (ar.compressDouble())
        {
            // If the matrix is of type double and compression is enabled, convert to float
            Eigen::Matrix<float, Type::RowsAtCompileTime, Type::ColsAtCompileTime> temp = val.template cast<float>();
            ar.append(reinterpret_cast<char*>(temp.data()), temp.size() * sizeof(float));
            return;
        }
    }
    ar.append(reinterpret_cast<char*>(val.data()), val.size() * sizeof(typename Type::Base::Scalar));
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, std::enable_if_t<is_eigen_mat<Type>::value, Type>& val)
{
    split(ar, val);
}

template <class Archive, typename Type>
inline void
dp::serialize(Archive& ar, std::enable_if_t<is_angle<Type>::value, Type>& val)
{
    ar & val();
}


template <class Archive, typename Type>
void
dp::load(Archive& ar, Configuration& c)
{
    std::string data;
    ar >> data;
    c = parseConfigString(data);
}

template <class Archive, typename Type>
void
dp::store(Archive& ar, Configuration& c)
{
    std::stringstream ss;
    // Write the configuration to a string stream
    ss << c.dump();
    ar << ss.str();
}

template <class Archive, typename Type>
void
dp::serialize(Archive& ar, Configuration& c)
{
    split(ar, c);
}

#endif /* UAVAP_CORE_DATAPRESENTATION_APDATAPRESENTATION_DETAIL_BASICSERIALIZATIONIMPL_HPP_ */
