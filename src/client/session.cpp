#include "session.h"
#include <core/checked_cast.h>
#include <core/algo.h>
#include <protocol/record_type.h>

using namespace app::core;

namespace app::client {
namespace {

std::vector<uint8_t> MakeGreetingsPayload(const std::string& clientId, uint32_t tokenCount)
{
    constexpr uint8_t Version{ 1 };

    std::vector<uint8_t> payload;
    payload.push_back(Version);
    payload.push_back(CheckedStaticCast<uint8_t>(clientId.length()));
    Append(payload, clientId);
    AppendIntegerInNetworkOrder(payload, tokenCount);

    return payload;
}

std::vector<uint8_t> MakeTokenPayload(const std::string& token)
{
    std::vector<uint8_t> payload;
    payload.push_back(CheckedStaticCast<uint8_t>(token.length()));
    Append(payload, token);

    return payload;
}

std::vector<uint8_t> MakeRecord(protocol::RecordType type, const std::vector<uint8_t>& payload = {})
{
    std::vector<uint8_t> record;
    AppendIntegerInNetworkOrder<uint16_t>(record, payload.size());
    record.push_back(type);
    Append(record, payload);
    return record;
}

std::vector<uint8_t> MakeGreetingsRecord(const std::string& clientId, uint32_t tokenCount)
{
    return MakeRecord(protocol::RecordType::Greetings, MakeGreetingsPayload(clientId, tokenCount));
}

std::vector<uint8_t> MakeReadyRecord()
{
    return MakeRecord(protocol::RecordType::Ready);
}

std::vector<uint8_t> MakeTokenRecord(const std::string& token)
{
    return MakeRecord(protocol::RecordType::Token, MakeTokenPayload(token));
}

} // namespace


Session::Session(
        const Options& options,
        boost::asio::ip::tcp::socket& socket,
        boost::asio::yield_context& yield)
    : m_options{ options }
    , m_socket{ socket }
    , m_yield{ yield }
    , m_tokenDataBase{ m_options.path }
{
}

void Session::SendGreetings()
{
    const auto greetingsRecord{ MakeGreetingsRecord(m_options.clientId, m_options.tokenCount) };
    boost::asio::async_write(m_socket, boost::asio::buffer(greetingsRecord), m_yield);
}

void Session::ReceiveReadyRecord()
{
    constexpr size_t HeaderSize{ 3 };
    std::array<uint8_t, HeaderSize> recordBuffer;

    boost::asio::async_read(m_socket, boost::asio::buffer(recordBuffer), boost::asio::transfer_exactly(HeaderSize), m_yield);
    ValidateReadyRecord(MakeConstBlobRange(recordBuffer.data(), recordBuffer.size()));
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
    CHECK(boost::range::equal(MakeReadyRecord(), record), "Expected ready record");
}

void Session::SendToken(const std::string& token)
{
    const auto tokenRecord{ MakeTokenRecord(token) };
    boost::asio::async_write(m_socket, boost::asio::buffer(tokenRecord), m_yield);
}

} // namespace app::client
