#include "connections.h"
#include "client_info.h"
#include <core/exception.h>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_set>

namespace app::server {

class ConnectionContainer : public IConnections
{
public:
    void CheckAddConnection(IClientInfo& clientInfo) final
    {
        std::lock_guard l{ m_mutex };
        auto result{ m_connections.insert(clientInfo.GetClientConnectionInfo()) };

        CHECK(result.second, "Such client connection parameters already exist");
    }

    void RemoveConnection(IClientInfo& clientInfo) final
    {
        std::lock_guard l{ m_mutex };
        m_connections.erase(clientInfo.GetClientConnectionInfo());
    }

private:
    std::mutex m_mutex;
    std::unordered_set<ClientConnectionInfo, ClientConnectionInfoHash> m_connections;
};


std::shared_ptr<IConnections> CreateConnections()
{
    return std::make_shared<ConnectionContainer>();
}

} // namespace app::server
