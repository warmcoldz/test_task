#pragma once

#include "endian.h"
#include <iterator>

namespace app::core {
namespace detail {

template <typename Container, typename Iterator>
void AppendImpl(Container& cont, Iterator begin, Iterator end)
{
    cont.insert(std::cend(cont), begin, end);
}

} // namespace detail

template <typename Container, typename Range>
void Append(Container& cont, const Range& range)
{
    detail::AppendImpl(cont, std::cbegin(range), std::cend(range));
}

template <typename Integer, typename Container>
void AppendIntegerInNetworkOrder(Container& cont, Integer value)
{
    static_assert(std::is_integral_v<Integer>);

    const Integer networkValue{ HostToNetwork(value) };
    const auto begin{ reinterpret_cast<const uint8_t*>(&networkValue) };
    detail::AppendImpl(cont, begin, begin + sizeof(networkValue));
}

} // namespace app::core
