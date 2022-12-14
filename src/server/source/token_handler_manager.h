#pragma once

#include "client_info.h"
#include "options.h"
#include "logger.h"
#include <string>
#include <memory>

namespace app::server {

struct ITokenHandlerManager
{
    virtual void HandleToken(std::shared_ptr<IClientInfo> client, std::string&& token) = 0;
    virtual ~ITokenHandlerManager() = default;
};

std::shared_ptr<ITokenHandlerManager> CreateTokenHandlerManager(std::shared_ptr<ILogger> logger, const Options& options);

} // namespace app::server
