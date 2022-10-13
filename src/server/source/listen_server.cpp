#include "listen_server.h"
#include "client_session.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>

namespace app::server {

class ListenServer : std::enable_shared_from_this<ListenServer>
{
public:
    ListenServer(std::shared_ptr<ILogger> logger, const Options& options)
        : m_logger{ std::move(logger) }
        , m_options{ options }
        , m_ioContext{ static_cast<int>(m_options.threadCount) }
    {
    }

public:
    void Run()
    {
        boost::asio::spawn(
            m_ioContext,
            [&](boost::asio::yield_context yield)
                {
                    m_logger->Log(Severity::Info) << "Start listening" << std::endl;

                    auto acceptor{ Listen() };

                    uint64_t sessionNumber{ 0 };
                    while (true)
                    {
                        boost::system::error_code ec;
                        boost::asio::ip::tcp::socket socket{ m_ioContext };

                        m_logger->Log(Severity::Info) << "Waiting for client" << std::endl;
                        acceptor.async_accept(socket, yield[ec]);
                        if (!ec)
                        {
                            ++sessionNumber;

                            auto logger{ CreateLoggerWithPrefix(m_logger, "Session[" + std::to_string(sessionNumber) + "]: ") };
                            logger->Log(Severity::Info) << "Created for " << socket.remote_endpoint() << std::endl;

                            std::make_shared<ClientSession>(std::move(logger), m_ioContext, std::move(socket))->Run();
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

private:
    boost::asio::ip::tcp::acceptor Listen()
    {
        boost::asio::ip::tcp::acceptor acceptor{ m_ioContext };
        boost::asio::ip::tcp::endpoint endpoint{ boost::asio::ip::make_address(m_options.ipAddress), m_options.port };
        acceptor.open(endpoint.protocol());
        acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();
        return acceptor;
    }

private:
    const std::shared_ptr<ILogger> m_logger;
    const Options m_options;
    boost::asio::io_context m_ioContext;
};


void RunServer(const Options& options)
{
    ListenServer s{ CreateLogger(), options };
    s.Run();
}

} // namespace app::server
