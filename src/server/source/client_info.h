#pragma once

#include <boost/functional/hash.hpp>
#include <string>
#include <tuple>

namespace app::server {    

struct ClientConnectionInfo
{
    std::string ip;
    uint16_t port;
    std::string clientId;

    bool operator==(const ClientConnectionInfo& index) const
    {
        return std::tie(ip, port, clientId) == std::tie(index.ip, index.port, index.clientId);
    }
};

struct ClientConnectionInfoHash
{
    std::size_t operator()(const ClientConnectionInfo& index) const
    {
        std::size_t seed{ 0 };
        boost::hash_combine(seed, index.ip);
        boost::hash_combine(seed, index.port);
        boost::hash_combine(seed, index.clientId);
        return seed;
    }
};

struct IClientInfo
{
    virtual void SetExpectedTokens(uint16_t count) = 0;
    virtual uint16_t GetExpectedTokenCount() const = 0;
    virtual void SetClientId(std::string clientId) = 0;
    virtual const ClientConnectionInfo& GetClientConnectionInfo() const = 0;
    virtual uint64_t GetSessionId() const = 0;

    virtual ~IClientInfo() = default;
};

class ClientInfo : public IClientInfo
{
public:
    ClientInfo(const std::string& ip, uint16_t port, uint64_t sessionId)
        : m_connectionInfo{ ip, port }
        , m_sessionId{ sessionId }
    {
    }

public:
    void SetExpectedTokens(uint16_t count)
    {
        m_expectedTokenCount = count;
    }

    uint16_t GetExpectedTokenCount() const
    {
        return m_expectedTokenCount;
    }

    void SetClientId(std::string clientId)
    {
        m_connectionInfo.clientId = std::move(clientId);
    }

    const ClientConnectionInfo& GetClientConnectionInfo() const
    {
        return m_connectionInfo;
    }

    uint64_t GetSessionId() const
    {
        return m_sessionId;
    }

private:
    ClientConnectionInfo m_connectionInfo;
    const uint64_t m_sessionId;
    uint16_t m_expectedTokenCount{ 0 };
};

} // namespace app::server
