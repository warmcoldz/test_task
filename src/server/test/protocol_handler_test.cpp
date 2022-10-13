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

struct SenderMock : ISender
{
    MOCK_METHOD(void, Send, (const std::vector<uint8_t>&));
};

class ProtocolHandlerTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        m_senderMock = std::make_unique<SenderMock>();
        m_senderMockPtr = m_senderMock.get();
    }

protected:
    std::unique_ptr<SenderMock> m_senderMock;
    SenderMock* m_senderMockPtr;
};

TEST_F(ProtocolHandlerTest, HandleClientTokens)
{
    std::array tokens{ "aabc", "wwq" };

    ProtocolHandler protocolHandler{
        std::make_shared<StubLogger>(),
        std::move(m_senderMock),
        boost::asio::ip::tcp::endpoint{ boost::asio::ip::make_address("127.0.0.1"), 33337 }
    };

    protocol::RecordBuilder recordBuilder;
    EXPECT_CALL(*m_senderMockPtr, Send(recordBuilder.MakeReadyRecord()));
    
    protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", tokens.size())));
    for (const auto& token : tokens)
    {
        protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeTokenRecord(token)));
    }
}

} // namespace app::server
