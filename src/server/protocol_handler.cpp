#include "protocol_handler.h"
#include <core/exception.h>
#include <core/endian.h>
#include <protocol/record_type.h>

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
        m_tokenCount = reader.Read<uint32_t>();
    }

public:
    const std::string& GetClientId() const
    {
        return m_clientId;
    }

    uint32_t GetTokenCount() const
    {
        return m_tokenCount;
    }

private:
    std::string m_clientId;
    uint32_t m_tokenCount;
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

    std::clog << "[Ready] sending" << std::endl;
    sendData(MakeReadyRecord());
    std::clog << "[Ready] sending DONE" << std::endl;

    m_state = State::WaitingToken;
}

void ProtocolHandler::ProcessToken(Record& record)
{
    CHECK(m_state == State::WaitingToken, "Expected Token record");

    TokenRecord tokenRecord{ record };
    std::clog << "[Token: " << tokenRecord.GetToken() << "] received" << std::endl;
}

void ProtocolHandler::ProcessUnexpectedRecord(Record& record)
{
    std::clog << "[Record " << record.GetType() << "] received" << std::endl;
    throw std::runtime_error{ "Unexpected record type" };
}

const std::vector<uint8_t>& ProtocolHandler::MakeReadyRecord()
{
    const static std::vector<uint8_t> ReadyRecord {
        0x00, 0x00, // RecordPayloadSize
        protocol::RecordType::Ready // Type
    };

    return ReadyRecord;
}

} // namespace app::server
