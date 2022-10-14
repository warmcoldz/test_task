#pragma once

#include "token_handler_manager.h"
#include "client_info.h"
#include "connections.h"
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
        std::shared_ptr<IClientInfo> clientInfo,
        std::shared_ptr<ITokenHandlerManager> tokenHandlerManager,
        std::shared_ptr<IConnections> connections,
        std::unique_ptr<ISender> sender);

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
    const std::shared_ptr<ITokenHandlerManager> m_tokenHandlerManager;
    const std::shared_ptr<IConnections> m_connections;
    const std::unique_ptr<ISender> m_sender;
    const std::shared_ptr<IClientInfo> m_clientInfo;

    State m_state{ State::WaitingGreetings };
    RecordParser m_recordParser;
    std::string m_clientId;
    uint16_t m_tokensToProcess{ 0 };
    std::unique_ptr<ExistingConnectionGuard> m_existingConnectionGuard;
};

} // namespace app::server
