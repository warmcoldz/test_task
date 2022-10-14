#include "listen_server.h"
#include "client_session.h"
#include "token_handler_manager.h"
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
            std::shared_ptr<ITokenHandlerManager> tokenHandlerManager,
            std::shared_ptr<IConnections> connections,
            const Options& options)
        : m_logger{ std::move(logger) }
        , m_tokenHandlerManager{ std::move(tokenHandlerManager )}
        , m_connections{ std::move(connections) }
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
                                std::make_shared<ClientInfo>(socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port(), sessionNumber),
                                m_tokenHandlerManager,
                                m_connections,
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
    const std::shared_ptr<ITokenHandlerManager> m_tokenHandlerManager;
    const std::shared_ptr<IConnections> m_connections;
    const Options m_options;
    boost::asio::io_context m_ioContext;
};


void RunServer(const Options& options)
{
    auto logger{ CreateLogger(options.consoleLog) };
    auto tokenHandlerManager{ CreateTokenHandlerManager(logger, options) };
    auto connections{ CreateConnections() };

    ListenServer s{ std::move(logger), std::move(tokenHandlerManager), std::move(connections), options };
    s.Run();
}

} // namespace app::server
