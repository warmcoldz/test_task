#pragma once

#include "client_info.h"
#include <core/exception.h>
#include <boost/functional/hash.hpp>
#include <string>
#include <tuple>
#include <unordered_set>

namespace app::server {

struct IConnections
{
    virtual void CheckAddConnection(IClientInfo& clientInfo) = 0;
    virtual void RemoveConnection(IClientInfo& clientInfo) = 0;
    virtual ~IConnections() = default;
};

class ExistingConnectionGuard
{
public:
    ExistingConnectionGuard(IConnections& connections, IClientInfo& clientInfo)
        : m_connections{ connections }
        , m_clientInfo{ clientInfo }
    {
        m_connections.CheckAddConnection(m_clientInfo);
    }

    ~ExistingConnectionGuard()
    {
        m_connections.RemoveConnection(m_clientInfo);
    }

private:
    IConnections& m_connections;
    IClientInfo& m_clientInfo;
};

std::shared_ptr<IConnections> CreateConnectionContainer();

} // namespace app::server
