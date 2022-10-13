#pragma once

#include <core/exception.h>
#include <boost/functional/hash.hpp>
#include <string>
#include <tuple>
#include <unordered_set>

namespace app::server {

struct IConnectionContainer
{
    virtual void RegisterConnection(const std::string& clientId, const std::string& ip, uint16_t port) = 0;
    virtual void UnregisterConnection(const std::string& clientId, const std::string& ip, uint16_t port) = 0;
    virtual ~IConnectionContainer() = default;
};

class ExistingConnectionGuard
{
public:
    ExistingConnectionGuard(
            IConnectionContainer& registrator,
            const std::string& clientId,
            const std::string& ip,
            uint16_t port)
        : m_registrator{ registrator }
        , m_clientId{ clientId }
        , m_ip{ ip }
        , m_port{ port }
    {
        m_registrator.RegisterConnection(m_clientId, m_ip, m_port);
    }

    ~ExistingConnectionGuard()
    {
        m_registrator.UnregisterConnection(m_clientId, m_ip, m_port);
    }

private:
    IConnectionContainer& m_registrator;
    const std::string& m_clientId;
    const std::string& m_ip;
    const uint16_t m_port;
};

std::shared_ptr<IConnectionContainer> CreateConnectionContainer();

} // namespace app::server
