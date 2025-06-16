/*
 * DataPresentation.h
 *
 *  Created on: Jul 4, 2019
 *      Author: mirco
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_DATAPRESENTATION_H_
#define UAVAP_CORE_DATAPRESENTATION_DATAPRESENTATION_H_

#include "cpsCore/Aggregation/AggregatableObject.hpp"
#include "cpsCore/Configuration/ConfigurableObject.hpp"
#include "cpsCore/Utilities/Packet.h"
#include "cpsCore/Utilities/DataPresentation/detail/ArchiveOptions.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerialization.h"
#include "cpsCore/Utilities/DataPresentation/detail/BasicSerializationImpl.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryFromArchiveImpl.hpp"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchive.h"
#include "cpsCore/Utilities/DataPresentation/detail/Binary/BinaryToArchiveImpl.hpp"

class DataPresentation : public AggregatableObject<>, public ConfigurableObject<ArchiveOptions>
{
public:
    static constexpr TypeId typeId = "data_presentation";

    DataPresentation() = default;

    template <typename Header>
    void
    addHeader(Packet& packet, const Header& header) const;

    template <typename Header>
    Header
    extractHeader(Packet& packet) const;

    template <typename Header>
    Header
    getHeader(const Packet& packet) const;

    // template<typename Type>
    // Packet
    // serialize(const Type& val) const;
    //
    // template<typename Type>
    // Packet&
    // serialize(const Type& val, Packet& packet) const;
    //
    // template<typename Type>
    // Type
    // deserialize(const Packet& packet) const;
    //
    // template<typename Type>
    // Type
    // extract(Packet& packet) const;

    template <typename... Types>
    Packet
    serialize(const Types&... vals) const
    {
        std::string str;
        BinaryToArchive archive(str, params);
        (archive << ... << vals);
        return Packet(str);
    }

    template <typename... Types>
    Packet&
    serialize(const Types&... vals, Packet& packet) const
    {
        BinaryToArchive archive(packet.getBuffer(), params);
        (archive << ... << vals);
        return packet;
    }

    template <typename... Types>
    auto
    deserialize(const Packet& packet) const
    {
        static_assert(sizeof...(Types) > 0,
                      "At least one type must be specified for extraction.");
        BinaryFromArchive archive(packet.getBuffer(), params);
        if constexpr (sizeof...(Types) == 1)
        {
            std::tuple_element_t<0, std::tuple<Types...>> t;
            archive >> t;
            return t;
        }
        else
        {
            std::tuple<Types...> t;
            extractTuple(t, archive, std::make_index_sequence<sizeof...(Types)>{});
            return t;
        }
    }

    template <typename... Types>
    auto
    extract(Packet& packet) const
    {
        static_assert(sizeof...(Types) > 0,
                      "At least one type must be specified for extraction.");
        BinaryFromArchive archive(packet.getBuffer(), params);
        if constexpr (sizeof...(Types) == 1)
        {
            std::tuple_element_t<0, std::tuple<Types...>> t;
            archive >> t;
            packet.consume(archive.getConsumed());
            return t;
        }
        else
        {
            std::tuple<Types...> t;
            extractTuple(t, archive, std::make_index_sequence<sizeof...(Types)>{});
            packet.consume(archive.getConsumed());
            return t;
        }
    }

private:
    template <typename Tuple, std::size_t... Is>
    void
    extractTuple(Tuple& t, BinaryFromArchive& archive, std::index_sequence<Is...>) const
    {
        ((archive >> std::get<Is>(t)), ...);
    }
};

template <typename Header>
inline void
DataPresentation::addHeader(Packet& packet, const Header& header) const
{
    std::string headerStr;
    BinaryToArchive archive(headerStr, params);
    archive << header;
    packet.prepend(headerStr);
}

template <typename Header>
inline Header
DataPresentation::getHeader(const Packet& packet) const
{
    Header header;
    BinaryFromArchive archive(packet.getBuffer(), params);
    archive >> header;
    return header;
}

template <typename Header>
inline Header
DataPresentation::extractHeader(Packet& packet) const
{
    Header header;
    BinaryFromArchive archive(packet.getBuffer(), params);
    archive >> header;
    packet.consume(archive.getConsumed());
    return header;
}

// template<typename Type>
// inline Packet
// DataPresentation::serialize(const Type& val) const
// {
// 	std::string str;
// 	BinaryToArchive archive(str, params);
// 	archive << val;
// 	return Packet(str);
// }
//
// template<typename Type>
// inline Packet&
// DataPresentation::serialize(const Type& val, Packet& packet) const
// {
// 	BinaryToArchive archive(packet.getBuffer(), params);
// 	archive << val;
// 	return packet;
// }
//
// template<typename Type>
// inline Type
// DataPresentation::deserialize(const Packet& packet) const
// {
// 	Type t;
// 	BinaryFromArchive archive(packet.getBuffer(), params);
// 	archive >> t;
// 	return t;
// }
//
// template<typename Type>
// inline Type
// DataPresentation::extract(Packet& packet) const
// {
// 	Type t;
// 	BinaryFromArchive archive(packet.getBuffer(), params);
// 	archive >> t;
// 	packet.consume(archive.getConsumed());
// 	return t;
// }

#endif /* UAVAP_CORE_DATAPRESENTATION_DATAPRESENTATION_H_ */
