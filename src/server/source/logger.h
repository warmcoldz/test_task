#pragma once

#include <ostream>
#include <string_view>
#include <memory>

namespace app::server {

enum class Severity
{
    Info,
    Error
};

struct ILogger
{
    virtual std::ostream& Log(Severity severity) noexcept = 0;
    ~ILogger() = default;
};

std::shared_ptr<ILogger> CreateLogger(bool enableConsoleLog);
std::shared_ptr<ILogger> CreateLoggerWithPrefix(std::shared_ptr<ILogger> logger, const std::string& prefix);

} // namespace app::server
