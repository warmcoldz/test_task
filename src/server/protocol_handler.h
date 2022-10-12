#pragma once

#include "record_parser.h"
#include <core/range.h>
#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include <iostream>

namespace app::server {

using Sender = std::function<void(const std::vector<uint8_t>&)>;

class ProtocolHandler
{
public:
    explicit ProtocolHandler(const boost::asio::ip::tcp::endpoint& endpoint);

public:
    void ProcessData(core::ConstBlobRange data, Sender sendData);

private:
    void ProcessGreetings(Record& frame, Sender sendData);
    void ProcessToken(Record& frame);
    void ProcessUnexpectedRecord(Record& frame);

private:
    enum class State
    {
        WaitingGreetings,
        WaitingToken
    };

    State m_state{ State::WaitingGreetings };
    RecordParser m_recordParser;

    std::string m_ipAddress;
    uint16_t m_port;

    std::string m_clientId;
    uint32_t m_tokensToProcess{ 0 };
};

} // namespace app::server
