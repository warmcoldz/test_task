#include "connection_container.h"
#include <core/exception.h>
#include <boost/functional/hash.hpp>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_set>

namespace app::server {

struct Index
{
    std::string clientId;
    std::string ip;
    uint16_t port;

    bool operator==(const Index& index) const
    {
        return std::tie(clientId, ip, port) == std::tie(index.clientId, index.ip, index.port);
    }
};

struct IndexHash
{
    std::size_t operator()(const Index& index) const
    {
        std::size_t seed{ 0 };
        boost::hash_combine(seed, index.clientId);
        boost::hash_combine(seed, index.ip);
        boost::hash_combine(seed, index.port);
        return seed;
    }
};

class ConnectionContainer : public IConnectionContainer
{
public:
    void RegisterConnection(const std::string& clientId, const std::string& ip, uint16_t port) final
    {
        std::lock_guard l{ m_mutex };
        auto result{ m_connections.insert(Index{clientId, ip, port}) };

        CHECK(result.second, "Such connection parameters already exist");
    }

    void UnregisterConnection(const std::string& clientId, const std::string& ip, uint16_t port) final
    {
        std::lock_guard l{ m_mutex };
        m_connections.erase(Index{clientId, ip, port});
    }

private:
    std::mutex m_mutex;
    std::unordered_set<Index, IndexHash> m_connections;
};


std::shared_ptr<IConnectionContainer> CreateConnectionContainer()
{
    return std::make_shared<ConnectionContainer>();
}

} // namespace app::server
