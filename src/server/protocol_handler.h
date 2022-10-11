#pragma once

#include <core/record_parser.h>
#include <core/range.h>
#include <vector>
#include <iostream>

namespace app::server {

using Sender = std::function<void(const std::vector<uint8_t>&)>;

class ProtocolHandler
{
public:
    void ProcessData(core::ConstBlobRange data, Sender sendData);

private:
    void ProcessGreetings(core::Record& frame, Sender sendData);
    void ProcessToken(core::Record& frame);
    void ProcessUnexpectedRecord(core::Record& frame);
    static const std::vector<uint8_t>& MakeReadyRecord();

private:
    enum class State
    {
        WaitingGreetings,
        WaitingToken
    };

    State m_state{ State::WaitingGreetings };
    core::RecordParser m_recordParser;
};

} // namespace app::server
