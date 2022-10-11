#pragma once

#include <string>
#include <cstdint>

namespace app::server {

struct Options
{
    uint16_t port;
    uint32_t threadCount;
    std::string ipAddress;
};

} // namespace app::server
