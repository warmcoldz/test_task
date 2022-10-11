#pragma once

#include "range.h"
#include "assert.h"
#include <type_traits>
#include <endian.h>

namespace app::core {

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

} // namespace app::core
