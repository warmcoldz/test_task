#pragma once

#include "record_parser.h"
#include <core/range.h>
#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include <iostream>

namespace app::server {

struct ISender
{
    virtual void Send(const std::vector<uint8_t>& data) = 0;
    virtual ~ISender() = default;
};

class ProtocolHandler
{
public:
    ProtocolHandler(std::unique_ptr<ISender> sender, const boost::asio::ip::tcp::endpoint& endpoint);

public:
    void ProcessData(core::ConstBlobRange data);

private:
    void ProcessGreetings(Record& frame);
    void ProcessToken(Record& frame);
    void ProcessUnexpectedRecord(Record& frame);

private:
    enum class State
    {
        WaitingGreetings,
        WaitingToken,
        Finished
    };

    const std::unique_ptr<ISender> m_sender;
    const std::string m_ipAddress;
    const uint16_t m_port;

    State m_state{ State::WaitingGreetings };
    RecordParser m_recordParser;

    std::string m_clientId;
    uint16_t m_tokensToProcess{ 0 };
};

} // namespace app::server
