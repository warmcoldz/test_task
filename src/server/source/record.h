#pragma once

#include <vector>
#include <cstdint>
#include <core/range.h>
#include <protocol/record_type.h>

namespace app::server {

class Record
{
public:
    Record(std::vector<uint8_t>&& payload, protocol::RecordType type)
        : m_payload(std::move(payload))
        , m_type{ type } 
    {
    }

public:
    protocol::RecordType GetType() const
    {
        return m_type;
    }

    core::ConstBlobRange GetPayload() const
    {
        return core::MakeConstBlobRange(m_payload);
    }

private:
    const std::vector<uint8_t> m_payload;
    const protocol::RecordType m_type;
};

} // namespace app::server
