#pragma once

#include "range.h"
#include "assert.h"
#include "exception.h"
#include <type_traits>
#include <iterator>
#include <endian.h>

namespace app::core {

template <typename Integer>
Integer HostToNetwork(Integer value)
{
    static_assert(
        std::is_same_v<uint8_t, Integer>
            || std::is_same_v<uint16_t, Integer>
            || std::is_same_v<uint32_t, Integer>
            || std::is_same_v<uint64_t, Integer>,
        "value must be unsigned integral");

    if (std::is_same_v<uint16_t, Integer>)
    {
        return ::htobe16(value);
    }
    else if (std::is_same_v<uint32_t, Integer>)
    {
        return ::htobe32(value);
    }
    else if (std::is_same_v<uint64_t, Integer>)
    {
        return ::htobe64(value);
    }
    
    return value;
}

template <typename Integer>
Integer NetworkToHost(Integer value)
{
    static_assert(
        std::is_same_v<uint8_t, Integer>
            || std::is_same_v<uint16_t, Integer>
            || std::is_same_v<uint32_t, Integer>
            || std::is_same_v<uint64_t, Integer>,
        "value must be unsigned integral");

    if (std::is_same_v<uint16_t, Integer>)
    {
        return ::be16toh(value);
    }
    else if (std::is_same_v<uint32_t, Integer>)
    {
        return ::be32toh(value);
    }
    else if (std::is_same_v<uint64_t, Integer>)
    {
        return ::be64toh(value);
    }
    
    return value;
}

template <typename Integer>
Integer NetworkRead(ConstBlobRange data, size_t offset = 0)
{
    static constexpr size_t IntegerSize{ sizeof(Integer) };
    ASSERT(offset + IntegerSize <= data.size());

    const Integer value{ *reinterpret_cast<const Integer*>(data.begin() + offset) };
    data.advance_begin(IntegerSize);
    return NetworkToHost(value);
}

class NetworkReader
{
public:
    explicit NetworkReader(ConstBlobRange& data)
        : m_data{ data }
    {
    }

    template <typename Integer>
    Integer Read()
    {
        CheckBoundaries(sizeof(Integer));

        const auto value{ NetworkRead<Integer>(m_data) };
        m_data.advance_begin(sizeof(Integer));
        return value;
    }

    std::string ReadAsString(size_t bytes)
    {
        CheckBoundaries(bytes);

        std::string value{ m_data.begin(), m_data.begin() + bytes };
        m_data.advance_begin(bytes);
        return value;
    }

private:
    void CheckBoundaries(size_t bytes)
    {
        CHECK(m_data.size() >= bytes, "Parsing error: not enough data to read");
    }

private:
    ConstBlobRange& m_data;
};

} // namespace app::core
