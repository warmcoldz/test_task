#include "protocol_handler.h"
#include <core/exception.h>

namespace app::server {

void ProtocolHandler::ProcessData(core::ConstBlobRange data, Sender sendData)
{
    while (!data.empty())
    {
        std::optional<core::Record> record{ m_recordParser.ProcessData(data) };
        if (record.has_value())
        {
            switch (record->GetType())
            {
                case core::Record::Greetings:
                    ProcessGreetings(*record, sendData);
                    break;

                case core::Record::Token:
                    ProcessToken(*record);
                    break;
                
                default:
                    ProcessUnexpectedRecord(*record);
                    break;
            }
        }
    }
}

void ProtocolHandler::ProcessGreetings(core::Record& record, Sender sendData)
{
    std::clog << "[Greetings] received" << std::endl;
    CHECK(m_state == State::WaitingGreetings, "Expected Greetings record");

    std::clog << "[Ready] sending" << std::endl;

    sendData(MakeReadyRecord());

    std::clog << "[Ready] sending done" << std::endl;
}

void ProtocolHandler::ProcessToken(core::Record& record)
{
    std::clog << "[Token] received" << std::endl;
    CHECK(m_state == State::WaitingToken, "Expected Token record");
}

void ProtocolHandler::ProcessUnexpectedRecord(core::Record& record)
{
    std::clog << "[Record 0x" << std::hex << record.GetType() << "] received" << std::endl;
    throw std::runtime_error{ "Unexpected record type" };
}

const std::vector<uint8_t>& ProtocolHandler::MakeReadyRecord()
{
    const static std::vector<uint8_t> ReadyRecord {
        0x00, 0x00, // RecordPayloadSize
        core::Record::Ready // Type
    };

    return ReadyRecord;
}

} // namespace app::server
