#include "options.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace app::server {

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:
    ClientSession(boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket&& socket);

public:
    void Run();

private:
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};

} // namespace app::server
