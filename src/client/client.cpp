#include "client.h"
#include "session.h"
#include "options.h"
#include "token_data_base.h"
#include <core/exception.h>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>

using namespace app::core;

namespace app::client {

Client::Client(const Options& options)
    : m_options{ options }
{
}

void Client::Run()
{
    boost::asio::spawn(
        m_ioContext,
        [this](boost::asio::yield_context yield)
            {
                try
                {
                    boost::asio::ip::tcp::socket socket{ m_ioContext };
                    Session session{m_options, socket, yield};

                    boost::asio::ip::tcp::resolver resolver{ m_ioContext };                    
                    const auto results{ resolver.async_resolve(m_options.ipAddress, std::to_string(m_options.port), yield) };
                    boost::asio::async_connect(socket, results, yield);
                    
                    session.SendGreetings();
                    session.ReceiveReadyRecord();
                    session.SendTokens();

                    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                }
            });

    m_ioContext.run();
}

} // namespace app::client