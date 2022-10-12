#include "record_parser.h"
#include <core/endian.h>
#include <core/assert.h>

using namespace app::core;

namespace app::server {

std::optional<Record> RecordParser::ProcessData(ConstBlobRange& data)
{
    if (!IsHeaderCollected())
    {
        if (!CollectHeader(data))
            return std::nullopt;
    }
    
    if (!CollectPayload(data))
        return std::nullopt;

    return CreateRecord();
}

bool RecordParser::IsHeaderCollected() const
{
    return m_headerBufferSize == RecordHeaderSize;
}

bool RecordParser::CollectHeader(ConstBlobRange& data)
{
    ASSERT(!IsHeaderCollected());

    const size_t appendHeaderSize{ std::min(RecordHeaderSize - m_headerBufferSize, data.size()) };
    std::copy(data.begin(), data.begin() + appendHeaderSize, std::begin(m_headerBuffer));
    data.advance_begin(appendHeaderSize);
    m_headerBufferSize += appendHeaderSize;

    if (!IsHeaderCollected())
        return false;

    m_expectedPayloadSize = NetworkRead<RecordPayloadSize>(m_headerBuffer);
    m_payloadBuffer.reserve(m_expectedPayloadSize);
    return true;
}

bool RecordParser::CollectPayload(ConstBlobRange& data)
{
    const size_t lengthToAppend{ std::min(m_expectedPayloadSize - m_payloadBuffer.size(), data.size()) };
    m_payloadBuffer.insert(m_payloadBuffer.cend(), data.begin(), data.begin() + lengthToAppend);
    data.advance_begin(lengthToAppend);

    return IsPayloadCollected();
}

Record RecordParser::CreateRecord()
{
    ASSERT(IsPayloadCollected());

    m_headerBufferSize = 0;
    m_expectedPayloadSize = 0;

    std::vector<uint8_t> payloadBuffer;
    payloadBuffer.swap(m_payloadBuffer);

    const auto type{ NetworkRead<uint8_t>(m_headerBuffer, /*offset*/ sizeof(RecordPayloadSize)) };
    return { std::move(payloadBuffer), static_cast<protocol::RecordType>(type) };
}

bool RecordParser::IsPayloadCollected() const
{
    return m_payloadBuffer.size() == m_expectedPayloadSize;
}

} // namespace app::server
