/*
 * SerialHandler.cpp
 *
 *  Created on: Jul 27, 2018
 *      Author: mircot
 */
#include "cpsCore/Utilities/IDC/Header/NetworkHeader.h"
#include "cpsCore/Utilities/IDC/NetworkLayer/Serial/SerialHandler.h"
#include "cpsCore/Utilities/DataPresentation/BinarySerialization.hpp"

SerialHandler::SerialHandler(const SerialHandlerParams& params) :
    io_(), serial_(io_, params.serialPort()), delim_(
        params.delimiterString()[params.delimiterString().size() - 1]), delimString_(
        params.delimiterString()), useCRC_(params.useCRC()), direction_(params.direction()), sendBlocking_(
        params.sendBlocking()), handlerCanceled_(false)
{
    serial_.set_option(boost::asio::serial_port_base::baud_rate(params.baudRate()));
    if (params.flowControl() != boost::asio::serial_port_base::flow_control::none)
        serial_.set_option(boost::asio::serial_port_base::flow_control(params.flowControl()));

    if (::tcflush(serial_.lowest_layer().native_handle(), TCIFLUSH) != 0)
    {
        CPSLOG_WARN << "Cannot flush serial port.";
    }

    if (direction_ != SerialDirection::SEND)
        boost::asio::async_read_until(serial_, inBuffer_, delim_,
                                      std::bind(&SerialHandler::receive, this, std::placeholders::_1,
                                                std::placeholders::_2));
}

SerialHandler::~SerialHandler()
{
    cancelHandler();
    serial_.close();
    CPSLOG_DEBUG << "Serial handler destroyed";
    CPSLogger::instance()->flush();
}

boost::signals2::connection
SerialHandler::subscribeOnPackets(const OnPacket::slot_type& slot)
{
    if (direction_ == SerialDirection::SEND)
    {
        CPSLOG_ERROR << "Handler only configured to send. Will not receive.";
        return boost::signals2::connection();
    }
    return onPacket_.connect(slot);
}

bool
SerialHandler::sendPacket(const Packet& packet)
{
    if (direction_ == SerialDirection::RECEIVE)
    {
        CPSLOG_ERROR << "Handler only configured to receive. Will not send.";
        return false;
    }

    std::string message = packet.getBuffer();

    if (useCRC_)
    {
        NetworkHeader header;
        header.crc = packet.getCRC16();
        message.insert(0, dp::serialize(header).getBuffer());
        CPSLOG_TRACE << "Send Header CRC: " << header.crc;
    }

    message += delimString_;
    stats_.dataSent() += message.size();
    stats_.packetsSent()++;

    std::ostream os(&outBuffer_);
    os << message;

    if (sendBlocking_)
        boost::asio::write(serial_, outBuffer_);
    else
        boost::asio::async_write(serial_, outBuffer_,
                                 boost::bind(&SerialHandler::sendStatus, this, boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

    return true;
}

void
SerialHandler::startHandler()
{
    while (!handlerCanceled_.load())
    {
        io_.run();
        if (!handlerCanceled_.load())
            io_.restart();
    }
    CPSLOG_DEBUG << "Serial handler canceled" << std::endl;
}

void
SerialHandler::resetStats()
{
    stats_ = NetworkStats();
}

void
SerialHandler::receive(const boost::system::error_code& err, std::size_t bytes_transferred)
{
    if (handlerCanceled_.load())
        return;
    if (err)
    {
        CPSLOG_ERROR << "Reception error: " << err.message();
        return;
    }
    CPSLOG_TRACE << "Serial received " << bytes_transferred << "bytes";
    std::istream read_buffer(&inBuffer_);
    std::string line;
    std::getline(read_buffer, line, delim_);

    boost::asio::async_read_until(serial_, inBuffer_, delim_,
                                  std::bind(&SerialHandler::receive, this, std::placeholders::_1,
                                            std::placeholders::_2));

    std::size_t delimSize = delimString_.size();
    std::string packetString;
    if (delimSize == 1)
    {
        packetString = line;
    }
    else
    {
        packetBuffer_ << line << delim_;

        packetString = packetBuffer_.str();

        if (packetString.size() >= delimString_.size()
            && delimString_.compare(
                packetString.substr(packetString.size() - delimSize,
                                    packetString.size() - 1)) == 0)
        {
            //End of packet
            packetString.erase(packetString.size() - delimSize, packetString.size() - 1);
            packetBuffer_.clear();
            packetBuffer_.str(std::string());
        }
        else
            return;
    }

    Packet packet(packetString);
    stats_.dataReceived() += packetString.size();
    stats_.packetsReceived()++;

    if (useCRC_)
    {
        NetworkHeader header = dp::extract<NetworkHeader>(packet);
        auto crc = packet.getCRC16();
        CPSLOG_TRACE << "Receive Header CRC: " << header.crc << ", Packet CRC: " << crc;
        if (header.crc != crc)
        {
            stats_.dataCorrupted() += packetString.size();
            stats_.packetsCorrupted()++;
            return;
        }
    }


    onPacket_(packet);
}

void
SerialHandler::sendStatus(const boost::system::error_code& ec, std::size_t bytes)
{
    CPSLOG_TRACE << "Serial " << bytes << " sent";
    if (ec)
    {
        CPSLOG_ERROR << "Error while sending: " << ec.message();
    }
}

void
SerialHandler::cancelHandler()
{
    handlerCanceled_.store(true);
    io_.stop();
}

const NetworkStats&
SerialHandler::getStats() const
{
    return stats_;
}
