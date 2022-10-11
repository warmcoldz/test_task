#pragma once

#include <vector>
#include <cstdint>

namespace app::core {

class Record
{
public:
    enum Type : uint8_t
    {
        Greetings = 0,
        Ready = 1,
        Token = 2
    };

public:
    Record(std::vector<uint8_t>&& payload, uint8_t type)
        : m_payload(std::move(payload))
        , m_type{ static_cast<Type>(type) } 
    {
    }

public:
    Type GetType() const
    {
        return m_type;
    }

private:
    const std::vector<uint8_t> m_payload;
    const Type m_type;
};

} // namespace app::core
