#include "session.h"
#include <core/checked_cast.h>
#include <core/algo.h>
#include <iostream>

using namespace app::core;

namespace app::client {

Session::Session(
        const Options& options,
        boost::asio::ip::tcp::socket& socket,
        boost::asio::yield_context& yield)
    : m_options{ options }
    , m_socket{ socket }
    , m_yield{ yield }
    , m_tokenDataBase{ m_options.path }
    , m_counter{ 0 }
{
}

void Session::SendGreetings()
{
    ++m_counter;

    std::clog << m_counter << ':' << "[Greetings: client_id=" << m_options.clientId << ", tokens=" << m_options.tokenCount << "] =>" << std::endl;

    const auto greetingsRecord{ m_recordBuilder.MakeGreetingsRecord(m_options.clientId, m_options.tokenCount) };
    boost::asio::async_write(m_socket, boost::asio::buffer(greetingsRecord), m_yield);

    std::clog << m_counter << ':' << "[Greetings] => OK" << std::endl;
}

void Session::ReceiveReadyRecord()
{
    ++m_counter;

    constexpr size_t HeaderSize{ 3 };
    std::array<uint8_t, HeaderSize> recordBuffer;

    boost::asio::async_read(m_socket, boost::asio::buffer(recordBuffer), boost::asio::transfer_exactly(HeaderSize), m_yield);
    ValidateReadyRecord(MakeConstBlobRange(recordBuffer.data(), recordBuffer.size()));

    std::clog << m_counter << ':' << "[Ready] <=" << std::endl;
}

void Session::SendTokens()
{
    for (uint32_t i{ 0 }; i < m_options.tokenCount; ++i)
    {
        auto token{ m_tokenDataBase.GetArbitraryToken() };
        SendToken(token);
    }
}

void Session::ValidateReadyRecord(ConstBlobRange record)
{
    CHECK(boost::range::equal(m_recordBuilder.MakeReadyRecord(), record), "Expected ready record");
}

void Session::SendToken(const std::string& token)
{
    ++m_counter;

    std::clog << m_counter << ':' << "[Token: " << token << "] =>" << std::endl;

    const auto tokenRecord{ m_recordBuilder.MakeTokenRecord(token) };
    boost::asio::async_write(m_socket, boost::asio::buffer(tokenRecord), m_yield);

    std::clog << m_counter << ':' << "[Token] => OK" << std::endl;
}

} // namespace app::client
