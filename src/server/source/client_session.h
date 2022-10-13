#include "options.h"
#include "token_handler.h"
#include "connection_container.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace app::server {

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
    ClientSession(
        std::shared_ptr<ILogger> logger,
        std::shared_ptr<ITokenHandler> tokenHandler,
        std::shared_ptr<IConnectionContainer> connectionRegistrator,
        boost::asio::io_context& ioContext,
        boost::asio::ip::tcp::socket&& socket);

public:
    void Run();

private:
    const std::shared_ptr<ILogger> m_logger;
    const std::shared_ptr<ITokenHandler> m_tokenHandler;
    const std::shared_ptr<IConnectionContainer> m_connectionRegistrator;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};

} // namespace app::server
