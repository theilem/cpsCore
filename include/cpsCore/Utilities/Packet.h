/**
 * @file Packet.h
 * @brief Defines the packet class
 * @date Jul 17, 2017
 * @author Mirco Theile, mirco.theile@tum.de
 */

#ifndef UAVAP_CORE_DATAPRESENTATION_PACKET_H_
#define UAVAP_CORE_DATAPRESENTATION_PACKET_H_

#include <string>

/**
 * @brief Describes the Packet class that contains a string representing data.
 */
class Packet
{

public:

	/**
	 * @brief Default constructor.
	 */
	Packet() = default;

	/**
	 * @brief Create a packet with a given string
	 * @param buf initial string of the packet
	 */
	Packet(const std::string& buf);

	/**
	 * @brief Add a string in front of the current buffer
	 * @param str String to be prepended
	 */
	void
	prepend(const std::string& str);

	void
	prepend(const Packet& str);

	/**
	 * @return Return a const reference to the buffer_
	 */
	const std::string&
	getBuffer() const;

	/**
	 * @return Return a reference to the buffer_
	 */
	std::string&
	getBuffer();

	/**
	 * @return Return a const char pointer to the starting character of buffer_
	 */
	const char*
	getStart() const;

	void*
	getStartAddress();

	/**
	 * @brief Get the size of the current buffer_.
	 * @return Size of the buffer
	 */
	std::size_t
	getSize() const;

	std::string
	consume(std::size_t length);

	std::uint16_t
	getCRC16() const;

	Packet
	hamming74Encode() const;

	Packet
	hamming74Decode() const;

private:

	std::string buffer_; //!< Buffer containing the information in serialized form
};

namespace dp
{
	template <class Archive, typename>
	void
	load(Archive& ar, Packet& packet)
	{
		std::string str;
		ar >> str;
		packet = Packet(str);
	}
	template <class Archive, typename>
	void
	store(Archive& ar, Packet& packet)
	{
		ar << packet.getBuffer();
	}
	template <class Archive, typename>
	void
	serialize(Archive& ar, Packet& packet)
	{
		split(ar, packet);
	}

}


#endif /* UAVAP_CORE_DATAPRESENTATION_PACKET_H_ */
