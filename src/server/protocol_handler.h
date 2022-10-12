#pragma once

#include <core/record_parser.h>
#include <core/range.h>
#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include <iostream>

namespace app::server {

using Sender = std::function<void(const std::vector<uint8_t>&)>;

class ProtocolHandler
{
public:
    explicit ProtocolHandler(const boost::asio::ip::tcp::endpoint& endpoint)
        : m_ipAddress{ endpoint.address().to_string() }
        , m_port{ endpoint.port() }
    {
        std::clog << m_ipAddress << ":" << m_port << std::endl;
    }

public:
    void ProcessData(core::ConstBlobRange data, Sender sendData);

private:
    void ProcessGreetings(core::Record& frame, Sender sendData);
    void ProcessToken(core::Record& frame);
    void ProcessUnexpectedRecord(core::Record& frame);
    static const std::vector<uint8_t>& MakeReadyRecord();

private:
    enum class State
    {
        WaitingGreetings,
        WaitingToken
    };

    State m_state{ State::WaitingGreetings };
    core::RecordParser m_recordParser;

    std::string m_ipAddress;
    uint16_t m_port;

    //int expectedTokens{ 0 };
    //std::string clientId;
};

} // namespace app::server
