#include "logger.h"
#include <core/assert.h>
#include <chrono>
#include <iostream>
#include <thread>

namespace app::server {

class Logger : public ILogger
{
public:
    std::ostream& Log(Severity severity) noexcept final
    {
        // TODO: Make async logger to reduce IO load and fix interleaving of messages

        std::clog
            << std::this_thread::get_id() << '\t'
            << GetSeverityText(severity) << '\t';

        return std::clog;
    }

private:
    static std::string_view GetSeverityText(Severity severity)
    {
        using namespace std::string_view_literals;

        switch (severity)
        {
            case Severity::Info:
                return "INF"sv;
            case Severity::Error:
                return "ERR"sv;
        }

        ASSERT(!"Unexpected severity");
        return {};
    }
};

class LoggerWithPrefix : public ILogger
{
public:
    LoggerWithPrefix(std::shared_ptr<ILogger> logger, const std::string& prefix)
        : m_logger{ std::move(logger) }
        , m_prefix{ prefix }
    {
    }

public:
    std::ostream& Log(Severity severity) noexcept final
    {
        return m_logger->Log(severity) << m_prefix;
    }

private:
    const std::shared_ptr<ILogger> m_logger;
    const std::string m_prefix;
};


std::shared_ptr<ILogger> CreateLogger()
{
    return std::make_shared<Logger>();
}

std::shared_ptr<ILogger> CreateLoggerWithPrefix(std::shared_ptr<ILogger> logger, const std::string& prefix)
{
    return std::make_shared<LoggerWithPrefix>(logger, prefix);
}

} // namespace app::server
