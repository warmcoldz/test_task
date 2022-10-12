#pragma once

#include <cstdint>

namespace app::protocol {

enum RecordType : uint8_t
{
    Greetings = 0,
    Ready = 1,
    Token = 2
};

} // namespace app::protocol
