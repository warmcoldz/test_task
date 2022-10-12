#pragma once

#include <boost/range/iterator_range.hpp>

namespace app::core {

using ConstBlobRange = boost::iterator_range<const uint8_t*>;

template <typename Range>
ConstBlobRange MakeConstBlobRange(const Range& data)
{
    return { std::begin(data), std::begin(data) + std::size(data) };
}

template <typename Iterator>
ConstBlobRange MakeConstBlobRange(Iterator begin, size_t size)
{
    return { begin, begin + size };
}

} // namespace app::core
