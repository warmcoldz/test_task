#pragma once

#include "endian.h"
#include <iterator>

namespace app::core {

template <typename Container, typename Range>
void Append(Container& cont, const Range& range)
{
    cont.insert(std::cend(cont), std::cbegin(range), std::cend(range));
}

template <typename Integer, typename Container>
void AppendIntegerInNetworkOrder(Container& cont, Integer value)
{
    static_assert(std::is_integral_v<Integer>);

    const Integer networkValue{ HostToNetwork(value) };

    const auto begin{ reinterpret_cast<const uint8_t*>(&networkValue) };
    cont.insert(std::cend(cont), begin, begin + sizeof(networkValue));
}

} // namespace app::core
