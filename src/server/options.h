#pragma once

#include <string>
#include <cstdint>

namespace app::server {

struct Options
{
    uint32_t threadCount;
    std::string ipAddress;
    uint16_t port;
};

} // namespace app::server
