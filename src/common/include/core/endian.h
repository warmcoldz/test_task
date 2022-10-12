#pragma once

#include <type_traits>
#include <iterator>
#include <endian.h>

namespace app::core {

template <typename Integer>
Integer HostToNetwork(Integer value)
{
    static_assert(
        std::is_integral_v<Integer> && std::is_unsigned_v<Integer>,
        "value must be unsigned integral");

    if constexpr (std::is_same_v<uint16_t, Integer>)
    {
        return ::htobe16(value);
    }
    else if constexpr (std::is_same_v<uint32_t, Integer>)
    {
        return ::htobe32(value);
    }
    else if constexpr (std::is_same_v<uint64_t, Integer>)
    {
        return ::htobe64(value);
    }
    
    return value;
}

template <typename Integer>
Integer NetworkToHost(Integer value)
{
    static_assert(
        std::is_integral_v<Integer> && std::is_unsigned_v<Integer>,
        "value must be unsigned integral");

    if constexpr (std::is_same_v<uint16_t, Integer>)
    {
        return ::be16toh(value);
    }
    else if constexpr (std::is_same_v<uint32_t, Integer>)
    {
        return ::be32toh(value);
    }
    else if constexpr (std::is_same_v<uint64_t, Integer>)
    {
        return ::be64toh(value);
    }
    
    return value;
}

} // namespace app::core
