#pragma once

#include "options.h"
#include <boost/asio.hpp>
#include <memory>

namespace app::server {

class ListenServer : std::enable_shared_from_this<ListenServer>
{
public:
    explicit ListenServer(const Options& options);

public:
    void Run();

private:
    boost::asio::ip::tcp::acceptor Listen();

private:
    const Options m_options;
    boost::asio::io_context m_ioContext;
};

} // namespace app::server
