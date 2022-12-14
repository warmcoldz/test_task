#include "client.h"
#include "session.h"
#include "options.h"
#include "token_data_base.h"
#include <core/exception.h>
#include <core/range.h>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>

using namespace app::core;

namespace app::client {

class Client
{
public:
    explicit Client(const Options& options)
        : m_options{ options }
    {
    }

public:
    void Run()
    {
        boost::asio::spawn(
            m_ioContext,
            [this](boost::asio::yield_context yield)
                {
                    try
                    {
                        boost::asio::ip::tcp::socket socket{ m_ioContext };
                        boost::asio::ip::tcp::resolver resolver{ m_ioContext };                    
                        const auto results{ resolver.async_resolve(m_options.ipAddress, std::to_string(m_options.port), yield) };

                        Session session{ m_options, socket, yield };
                        std::clog << "Connecting to server: " << m_options.ipAddress << ":" << m_options.port << std::endl;
                        boost::asio::async_connect(socket, results, yield);
                        std::clog << "Connecting to server OK" << std::endl;

                        session.SendGreetings();
                        session.ReceiveReadyRecord();
                        session.SendTokens();

                        std::clog << "Graceful shutdown" << std::endl;
                        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << std::endl;
                    }
                });

        m_ioContext.run();
    }

private:
    const Options m_options;
    boost::asio::io_context m_ioContext;
};


void RunClient(const Options& options)
{
    Client client{ options };
    client.Run();
}

} // namespace app::client
