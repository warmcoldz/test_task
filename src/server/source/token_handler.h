#pragma once

#include "client_info.h"
#include "logger.h"
#include <string>
#include <ostream>
#include <vector>

namespace app::server {

struct ClientState
{
    uint64_t sessionId{ 0 };
    uint64_t expected{ 0 };
    uint64_t received{ 0 };
};

struct ClientTokenInfo
{
    ClientConnectionInfo clientInfo;
    ClientState clientState;

    friend std::ostream& operator<<(std::ostream& os, const ClientTokenInfo& state)
    {
        os
            << state.clientInfo.clientId << " (" << state.clientInfo.ip << ':' << state.clientInfo.port << "): "
            << "expected=" << state.clientState.expected << ", received=" << state.clientState.received;
        return os;
    }
};

struct ITokenHandler
{
    virtual void Handle(const IClientInfo& client) = 0;
    virtual std::vector<ClientTokenInfo> GetTokenState() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual ~ITokenHandler() = default;
};

std::shared_ptr<ITokenHandler> CreateTokenHandler(std::shared_ptr<ILogger> logger, const std::string& token);

} // namespace app::server
