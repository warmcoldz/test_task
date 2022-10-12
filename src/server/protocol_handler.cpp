#include "protocol_handler.h"
#include <core/exception.h>
#include <core/network_reader.h>
#include <protocol/record_builder.h>

using namespace app::core;

namespace app::server {

class GreetingsRecord
{
public:
    explicit GreetingsRecord(const Record& record)
    {
        auto range{ record.GetPayload() };

        NetworkReader reader{ range };
        const auto version{ reader.Read<uint8_t>() };
        CHECK(version == 1, "Unexpected version");
        const auto clientIdLength{ reader.Read<uint8_t>() };
        m_clientId = reader.ReadAsString(clientIdLength);
        m_tokenCount = reader.Read<uint16_t>();
    }

public:
    const std::string& GetClientId() const
    {
        return m_clientId;
    }

    uint16_t GetTokenCount() const
    {
        return m_tokenCount;
    }

private:
    std::string m_clientId;
    uint16_t m_tokenCount;
};

class TokenRecord
{
public:
    explicit TokenRecord(const Record& record)
    {
        auto range{ record.GetPayload() };

        NetworkReader reader{ range };
        const auto length{ reader.Read<uint8_t>() };
        m_token = reader.ReadAsString(length);
    }

    std::string&& GetToken()
    {
        return std::move(m_token);
    }

private:
    std::string m_token;
};


ProtocolHandler::ProtocolHandler(const boost::asio::ip::tcp::endpoint& endpoint)
    : m_ipAddress{ endpoint.address().to_string() }
    , m_port{ endpoint.port() }
{
    std::clog << "Accepted connection from: " << endpoint << std::endl;
}

void ProtocolHandler::ProcessData(ConstBlobRange data, Sender sendData)
{
    while (!data.empty())
    {
        std::optional<Record> record{ m_recordParser.ProcessData(data) };
        if (record.has_value())
        {
            switch (record->GetType())
            {
                case protocol::RecordType::Greetings:
                    ProcessGreetings(*record, sendData);
                    break;

                case protocol::RecordType::Token:
                    ProcessToken(*record);
                    break;
                
                default:
                    ProcessUnexpectedRecord(*record);
                    break;
            }
        }
    }
}

void ProtocolHandler::ProcessGreetings(Record& record, Sender sendData)
{
    CHECK(m_state == State::WaitingGreetings, "Expected Greetings record");

    GreetingsRecord greetingsRecord{ record };
    std::clog
        << "[Greetings: client_id=" << greetingsRecord.GetClientId()
        << ", tokens=" << greetingsRecord.GetTokenCount()
        << "] received"
        << std::endl;

    protocol::RecordBuilder recordBuilder;

    std::clog << "[Ready] sending" << std::endl;
    sendData(recordBuilder.MakeReadyRecord());
    std::clog << "[Ready] sending DONE" << std::endl;

    m_clientId = greetingsRecord.GetClientId();
    m_tokensToProcess = greetingsRecord.GetTokenCount();
    m_state = State::WaitingToken;
}

void ProtocolHandler::ProcessToken(Record& record)
{
    CHECK(m_state == State::WaitingToken, "Unexpected token record - waiting other record");
    CHECK(m_tokensToProcess != 0, "Unexpected token - all tokens handled");

    TokenRecord tokenRecord{ record };
    std::clog << "[Token: " << tokenRecord.GetToken() << "] received" << std::endl;
    --m_tokensToProcess;
}

void ProtocolHandler::ProcessUnexpectedRecord(Record& record)
{
    std::clog << "[Record " << record.GetType() << "] received" << std::endl;
    throw std::runtime_error{ "Unexpected record type" };
}

} // namespace app::server
