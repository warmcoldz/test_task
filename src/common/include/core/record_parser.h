#pragma once

#include "record.h"
#include "range.h"
#include <optional>
#include <array>
#include <vector>

namespace app::core {

using RecordPayloadSize = uint16_t;
inline constexpr size_t RecordHeaderSize{ sizeof(RecordPayloadSize) + sizeof(uint8_t) };

class RecordParser
{
public:
    std::optional<Record> ProcessData(ConstBlobRange& data);

private:
    bool IsHeaderCollected() const;
    bool CollectHeader(ConstBlobRange& data);
    bool CollectPayload(ConstBlobRange& data);
    Record CreateRecord();
    bool IsPayloadCollected() const;

private:
    size_t m_headerBufferSize{ 0 };
    RecordPayloadSize m_expectedPayloadSize{ 0 };

    std::array<uint8_t, RecordHeaderSize> m_headerBuffer;
    std::vector<uint8_t> m_payloadBuffer;
};

} // namespace app::core
