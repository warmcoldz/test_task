#include "listen_server.h"
#include "client_session.h"
#include "token_handler.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>

namespace app::server {

class ListenServer : std::enable_shared_from_this<ListenServer>
{
public:
    ListenServer(
            std::shared_ptr<ILogger> logger,
            std::shared_ptr<ITokenHandler> tokenHandler,
            std::shared_ptr<IConnectionContainer> connectionRegistrator,
            const Options& options)
        : m_logger{ std::move(logger) }
        , m_tokenHandler{ std::move(tokenHandler )}
        , m_connectionRegistrator{ std::move(connectionRegistrator) }
        , m_options{ options }
    {
    }

public:
    void Run()
    {
        QueueAccept();
        SpawnAcceptorThreads();
    }

private:
    void QueueAccept()
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

                            auto sessionLogger{ CreateLoggerWithPrefix(m_logger, "Session[" + std::to_string(sessionNumber) + "]: ") };
                            sessionLogger->Log(Severity::Info) << "Created for " << socket.remote_endpoint() << std::endl;

                            std::make_shared<ClientSession>(
                                std::move(sessionLogger),
                                m_tokenHandler,
                                m_connectionRegistrator,
                                m_ioContext,
                                std::move(socket))->Run();
                        }
                    }
                });
    }

    void SpawnAcceptorThreads()
    {
        constexpr int AdditionalThreadCount{3};

        std::vector<std::thread> threads;
        threads.reserve(AdditionalThreadCount);

        for (int i{ 0 }; i < AdditionalThreadCount; ++i)
        {
            threads.emplace_back([&]{ m_ioContext.run(); });
        }
        
        m_ioContext.run();

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

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
    const std::shared_ptr<ITokenHandler> m_tokenHandler;
    const std::shared_ptr<IConnectionContainer> m_connectionRegistrator;
    const Options m_options;
    boost::asio::io_context m_ioContext;
};


void RunServer(const Options& options)
{
    auto logger{ CreateLogger() };
    auto tokenHandler{ CreateTokenHandler(logger, options) };
    auto connectionRegistrator{ CreateConnectionContainer() };

    ListenServer s{ std::move(logger), std::move(tokenHandler), std::move(connectionRegistrator), options };
    s.Run();
}

} // namespace app::server
