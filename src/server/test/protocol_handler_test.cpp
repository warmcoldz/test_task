#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <source/protocol_handler.h>
#include <protocol/record_builder.h>

using namespace app::core;

namespace app::server {

struct StubLogger : ILogger
{
    std::ostream& Log(Severity severity) noexcept final
    {
        return std::clog;
    }
};

struct MockTokenHandler : ITokenHandler
{
    MOCK_METHOD(void, HandleToken, (std::string&&));
};

struct MockSender : ISender
{
    MOCK_METHOD(void, Send, (const std::vector<uint8_t>&));
};

struct StubConnectionContainer : IConnectionContainer
{
    void RegisterConnection(const std::string&, const std::string&, uint16_t)
    {
    }

    void UnregisterConnection(const std::string&, const std::string&, uint16_t)
    {
    }
};

constexpr uint16_t DefaultPort{ 12345 };
const auto DefaultIp{ boost::asio::ip::make_address("192.168.1.2") };
const auto AnotherIp{ boost::asio::ip::make_address("192.168.1.3") };
const boost::asio::ip::tcp::endpoint DefaultEndpoint{ DefaultIp, DefaultPort };

class ProtocolHandlerTest : public ::testing::Test
{
public:
    void SetUp() final
    {
        m_tokenHandlerMock = std::make_shared<MockTokenHandler>();
    }

    void TearDown() final
    {
        m_tokenHandlerMock.reset();
    }

protected:
    std::unique_ptr<::testing::NiceMock<MockSender>> CreateMockSender()
    {
        return std::make_unique<::testing::NiceMock<MockSender>>();
    }

protected:
    std::shared_ptr<MockTokenHandler> m_tokenHandlerMock;
};

TEST_F(ProtocolHandlerTest, HandleClientTokens)
{
    std::array tokens{ "aabc", "wwq" };

    auto sender{ CreateMockSender() };
    auto& senderRef{ *sender };

    ProtocolHandler protocolHandler{
        std::make_shared<StubLogger>(),
        m_tokenHandlerMock,
        std::make_shared<StubConnectionContainer>(),
        std::move(sender),
        boost::asio::ip::tcp::endpoint{ DefaultIp, 12345 }
    };

    protocol::RecordBuilder recordBuilder;
    EXPECT_CALL(senderRef, Send(recordBuilder.MakeReadyRecord()));
    
    protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", tokens.size())));
    for (const auto& token : tokens)
    {
        EXPECT_CALL(*m_tokenHandlerMock, HandleToken(std::string{token})).Times(1);
        protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeTokenRecord(token)));
    }
}

TEST_F(ProtocolHandlerTest, SameClientIdDifferentConnectionParameters)
{
    protocol::RecordBuilder recordBuilder;

    auto connectionContainer{ CreateConnectionContainer() };

    ProtocolHandler handler1{
        std::make_shared<StubLogger>(),
        m_tokenHandlerMock,
        connectionContainer,
        CreateMockSender(),
        DefaultEndpoint
    };

    ProtocolHandler handler2{
        std::make_shared<StubLogger>(),
        m_tokenHandlerMock,
        connectionContainer,
        CreateMockSender(),
        boost::asio::ip::tcp::endpoint{ AnotherIp, 12346 }
    };

    handler1.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", 1)));
    handler2.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", 1)));
}

TEST_F(ProtocolHandlerTest, SameAllClientParameters)
{
    protocol::RecordBuilder recordBuilder;

    auto connectionContainer{ CreateConnectionContainer() };
    auto logger{ std::make_shared<StubLogger>() };

    ProtocolHandler handler1{
        logger,
        m_tokenHandlerMock,
        connectionContainer,
        CreateMockSender(),
        DefaultEndpoint
    };

    ProtocolHandler handler2{
        std::move(logger),
        m_tokenHandlerMock,
        connectionContainer,
        CreateMockSender(),
        DefaultEndpoint
    };

    const auto greetingsRecord{ recordBuilder.MakeGreetingsRecord("client", 1) };
    handler1.ProcessData(MakeConstBlobRange(greetingsRecord));
    ASSERT_THROW(handler2.ProcessData(MakeConstBlobRange(greetingsRecord)), std::runtime_error);
}

} // namespace app::server
