#pragma once

#include "token_handler.h"
#include "connection_container.h"
#include "logger.h"
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
    ProtocolHandler(
        std::shared_ptr<ILogger> logger,
        std::shared_ptr<ITokenHandler> tokenHandler,
        std::shared_ptr<IConnectionContainer> connectionRegistrator,
        std::unique_ptr<ISender> sender,
        const boost::asio::ip::tcp::endpoint& endpoint);

    ~ProtocolHandler();

public:
    void ProcessData(core::ConstBlobRange data);

private:
    void ProcessGreetings(Record& record);
    void ProcessToken(Record& record);
    void ProcessUnexpectedRecord(Record& record);

private:
    enum class State
    {
        WaitingGreetings,
        WaitingToken,
        Finished
    };

    const std::shared_ptr<ILogger> m_logger;
    const std::shared_ptr<ITokenHandler> m_tokenHandler;
    const std::shared_ptr<IConnectionContainer> m_connectionRegistrator;
    const std::unique_ptr<ISender> m_sender;
    const std::string m_ipAddress;
    const uint16_t m_port;

    State m_state{ State::WaitingGreetings };
    RecordParser m_recordParser;
    std::string m_clientId;
    uint16_t m_tokensToProcess{ 0 };
    std::unique_ptr<ExistingConnectionGuard> m_existingConnectionGuard;
};

} // namespace app::server
