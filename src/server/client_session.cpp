#include "client_session.h"
#include "protocol_handler.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>
#include <array>

namespace app::server {

ClientSession::ClientSession(boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket&& socket)
    : m_socket{ std::move(socket) }
    , m_strand{ ioContext.get_executor() }
{
}

void ClientSession::Run()
{
    auto self{ shared_from_this() };
    boost::asio::spawn(
        m_strand,
        [this, self](boost::asio::yield_context yield)
            {
                try
                {
                    static constexpr size_t BufferSize{ 1024 };
                    std::array<uint8_t, BufferSize> buffer;

                    ProtocolHandler handler{ m_socket.remote_endpoint() };

                    auto sender{ [&](const std::vector<uint8_t>& range) {
                            boost::asio::async_write(m_socket, boost::asio::buffer(range), yield);
                        } };

                    while (true)
                    {
                        const std::size_t n{ m_socket.async_read_some(boost::asio::buffer(buffer, BufferSize), yield) };

                        const auto data{ core::MakeConstBlobRange(std::begin(buffer), n) };
                        handler.ProcessData(data, sender);
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << std::endl;
                    m_socket.close();
                    std::cerr << "Connection closed" << std::endl;
                }
            });
}

} // namespace app::server
