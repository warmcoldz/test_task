#include "listen_server.h"
#include "client_session.h"
#include <boost/asio/spawn.hpp>

namespace app::server {

ListenServer::ListenServer(const Options& options)
    : m_options{ options }
    , m_ioContext{ static_cast<int>(m_options.threadCount) }
{
}

void ListenServer::Run()
{
    boost::asio::spawn(
        m_ioContext,
        [&](boost::asio::yield_context yield)
            {
                auto acceptor{ Listen() };
                while (true)
                {
                    boost::system::error_code ec;
                    boost::asio::ip::tcp::socket socket{ m_ioContext };
                    acceptor.async_accept(socket, yield[ec]);
                    if (!ec)
                    {
                        std::make_shared<ClientSession>(m_ioContext, std::move(socket))->Run();
                    }
                }
            });

    std::vector<std::thread> threads;
    threads.reserve(m_options.threadCount);

    for (uint32_t i{ 0 }; i < m_options.threadCount; ++i)
    {
        threads.emplace_back([&]{ m_ioContext.run(); });
    }
    
    for (auto& thread : threads)
    {
        thread.join();
    }
}

boost::asio::ip::tcp::acceptor ListenServer::Listen()
{
    boost::asio::ip::tcp::acceptor acceptor{ m_ioContext };
    boost::asio::ip::tcp::endpoint endpoint{ boost::asio::ip::make_address(m_options.ipAddress), m_options.port };
    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen();
    return acceptor;
}

} // namespace app::server
