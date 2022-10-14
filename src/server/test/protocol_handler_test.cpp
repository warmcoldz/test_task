#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <source/protocol_handler.h>
#include <source/client_info.h>
#include <protocol/record_builder.h>

using namespace app::core;
using namespace ::testing;
using namespace std::string_literals;

namespace app::server {

struct StubLogger : ILogger
{
    std::ostream& Log(Severity severity) noexcept final
    {
        return std::clog;
    }
};

struct StubConnections : IConnections
{
    void CheckAddConnection(IClientInfo& clientInfo) final { };
    void RemoveConnection(IClientInfo& clientInfo) final { } ;
};

struct MockClientInfo : IClientInfo
{
    MOCK_METHOD(void, SetExpectedTokens, (uint16_t));
    MOCK_METHOD(uint16_t, GetExpectedTokenCount, (), (const));
    MOCK_METHOD(void, SetClientId, (std::string));
    MOCK_METHOD(const ClientConnectionInfo&, GetClientConnectionInfo, (), (const));
    MOCK_METHOD(uint64_t, GetSessionId, (), (const));
};

struct MockTokenHandlerManager : ITokenHandlerManager
{
    MOCK_METHOD(void, HandleToken, (std::shared_ptr<IClientInfo>, std::string&&));
};

struct MockSender : ISender
{
    MOCK_METHOD(void, Send, (const std::vector<uint8_t>&));
};

constexpr uint16_t DefaultPort{ 12345 };
const auto DefaultIp{ "192.168.1.2"s };
const auto AnotherIp{ "192.168.1.3"s };

class ProtocolHandlerTest : public Test
{
public:
    void SetUp() final
    {
        m_tokenHandlerManager = std::make_shared<NiceMock<MockTokenHandlerManager>>();
        m_stubLogger = std::make_shared<StubLogger>();
    }

    void TearDown() final
    {
        m_stubLogger.reset();
        m_tokenHandlerManager.reset();
    }

protected:
    std::unique_ptr<NiceMock<MockSender>> CreateMockSender()
    {
        return std::make_unique<NiceMock<MockSender>>();
    }

protected:
    std::shared_ptr<NiceMock<MockTokenHandlerManager>> m_tokenHandlerManager;
    std::shared_ptr<StubLogger> m_stubLogger;
};

TEST_F(ProtocolHandlerTest, HandleClientTokens)
{
    std::array tokens{ "aabc", "wwq" };

    auto sender{ CreateMockSender() };
    auto& senderRef{ *sender };

    ProtocolHandler protocolHandler{
        m_stubLogger,
        std::make_shared<NiceMock<MockClientInfo>>(),
        m_tokenHandlerManager,
        std::make_shared<StubConnections>(),
        std::move(sender)
    };

    protocol::RecordBuilder recordBuilder;
    EXPECT_CALL(senderRef, Send(recordBuilder.MakeReadyRecord()));
    
    protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", tokens.size())));
    for (const auto& token : tokens)
    {
        EXPECT_CALL(*m_tokenHandlerManager, HandleToken(_, std::string{token})).Times(1);
        protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeTokenRecord(token)));
    }
}

TEST_F(ProtocolHandlerTest, SameClientIdDifferentConnectionParameters)
{
    protocol::RecordBuilder recordBuilder;

    auto connectionContainer{ CreateConnections() };

    ProtocolHandler handler1{
        m_stubLogger,
        std::make_shared<ClientInfo>(DefaultIp, DefaultPort, 1),
        m_tokenHandlerManager,
        connectionContainer,
        CreateMockSender()
    };

    ProtocolHandler handler2{
        m_stubLogger,
        std::make_shared<ClientInfo>(AnotherIp, DefaultPort, 2),
        m_tokenHandlerManager,
        connectionContainer,
        CreateMockSender()
    };

    handler1.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", 1)));
    handler2.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", 1)));
}

TEST_F(ProtocolHandlerTest, SameAllClientParameters)
{
    protocol::RecordBuilder recordBuilder;

    auto connectionContainer{ CreateConnections() };

    ProtocolHandler handler1{
        m_stubLogger,
        std::make_shared<ClientInfo>(DefaultIp, DefaultPort, 1),
        m_tokenHandlerManager,
        connectionContainer,
        CreateMockSender()
    };

    ProtocolHandler handler2{
        m_stubLogger,
        std::make_shared<ClientInfo>(DefaultIp, DefaultPort, 1),
        m_tokenHandlerManager,
        connectionContainer,
        CreateMockSender(),
    };

    const auto greetingsRecord{ recordBuilder.MakeGreetingsRecord("client", 1) };
    handler1.ProcessData(MakeConstBlobRange(greetingsRecord));
    ASSERT_THROW(handler2.ProcessData(MakeConstBlobRange(greetingsRecord)), std::runtime_error);
}

} // namespace app::server
