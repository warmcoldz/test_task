#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <source/protocol_handler.h>
#include <protocol/record_builder.h>

using namespace app::core;

namespace app::server {

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
    ProtocolHandler protocolHandler{ 
        std::move(m_senderMock),
        boost::asio::ip::tcp::endpoint{ boost::asio::ip::make_address("127.0.0.1"), 33337 }
    };

    protocol::RecordBuilder recordBuilder;
    EXPECT_CALL(*m_senderMockPtr, Send(recordBuilder.MakeReadyRecord()));
    protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeGreetingsRecord("client", 2)));
    protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeTokenRecord("aabc")));
    protocolHandler.ProcessData(MakeConstBlobRange(recordBuilder.MakeTokenRecord("wwq")));
}

} // namespace app::server
