#pragma once

#include <string>
#include <cstdint>

namespace app::client {

struct Options
{
    std::string clientId;
    std::string path;
    uint16_t tokenCount;
    std::string ipAddress;
    uint32_t port;
};

} // namespace app::client
