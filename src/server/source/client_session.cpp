#include "client_session.h"
#include "protocol_handler.h"
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <iostream>
#include <array>

namespace app::server {

class Sender : public ISender
{
public:
    Sender(boost::asio::ip::tcp::socket& socket, boost::asio::yield_context& yield)
        : m_socket{ socket }
        , m_yield{ yield }
    {
    }

public:
    void Send(const std::vector<uint8_t>& data)
    {
        boost::asio::async_write(m_socket, boost::asio::buffer(data), m_yield);
    }

private:
    boost::asio::ip::tcp::socket& m_socket;
    boost::asio::yield_context& m_yield;
};


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

                    ProtocolHandler handler{ std::make_unique<Sender>(m_socket, yield), m_socket.remote_endpoint() };
                    while (true)
                    {
                        const std::size_t n{ m_socket.async_read_some(boost::asio::buffer(buffer, BufferSize), yield) };
                        handler.ProcessData(core::MakeConstBlobRange(std::begin(buffer), n));
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