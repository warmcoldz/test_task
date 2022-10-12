#pragma once

#include "exception.h"
#include <type_traits>
#include <limits>

namespace app::core {

template <typename OutputInteger, typename InputInteger>
OutputInteger CheckedStaticCast(InputInteger value)
{
    static_assert(std::is_integral_v<InputInteger>);
    static_assert(std::is_integral_v<OutputInteger>);
    static_assert(std::is_signed_v<InputInteger> == std::is_signed_v<OutputInteger>);

    CHECK(
        value >= std::numeric_limits<OutputInteger>::min() &&
        value <= std::numeric_limits<OutputInteger>::max(),
            "Static cast failed for " + std::to_string(value));

    return static_cast<OutputInteger>(value);
}

} // namespace app::core
