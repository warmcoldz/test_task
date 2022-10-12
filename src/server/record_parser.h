#pragma once

#include "record.h"
#include <core/range.h>
#include <optional>
#include <array>
#include <vector>

namespace app::server {

class RecordParser
{
public:
    std::optional<Record> ProcessData(core::ConstBlobRange& data);

private:
    bool IsHeaderCollected() const;
    bool CollectHeader(core::ConstBlobRange& data);
    bool CollectPayload(core::ConstBlobRange& data);
    Record CreateRecord();
    bool IsPayloadCollected() const;

private:
    using RecordPayloadSize = uint16_t;
    static constexpr size_t RecordHeaderSize{ sizeof(RecordPayloadSize) + sizeof(protocol::RecordType) };

    size_t m_headerBufferSize{ 0 };
    RecordPayloadSize m_expectedPayloadSize{ 0 };

    std::array<uint8_t, RecordHeaderSize> m_headerBuffer;
    std::vector<uint8_t> m_payloadBuffer;
};

} // namespace app::server
