#include "options.h"
#include "token_handler_manager.h"
#include "connections.h"
#include "client_info.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace app::server {

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
    ClientSession(
        std::shared_ptr<ILogger> logger,
        std::shared_ptr<IClientInfo> clientInfo,
        std::shared_ptr<ITokenHandlerManager> tokenHandlerManager,
        std::shared_ptr<IConnections> connections,
        boost::asio::io_context& ioContext,
        boost::asio::ip::tcp::socket&& socket);

public:
    void Run();

private:
    const std::shared_ptr<ILogger> m_logger;
    const std::shared_ptr<IClientInfo> m_clientInfo;
    const std::shared_ptr<ITokenHandlerManager> m_tokenHandlerManager;
    const std::shared_ptr<IConnections> m_connections;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};

} // namespace app::server
