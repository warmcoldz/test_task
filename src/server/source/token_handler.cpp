#include "token_handler.h"

namespace app::server {

class TokenHandlerManager : public ITokenHandler
{
public:
    explicit TokenHandlerManager(std::shared_ptr<ILogger> logger)
        : m_logger{ std::move(logger) }
    {
    }

public:
    void HandleToken(std::string&& token) final
    {
        m_logger->Log(Severity::Info) << "Handle " << token << std::endl;
    }

private:
    const std::shared_ptr<ILogger> m_logger;
};


std::shared_ptr<ITokenHandler> CreateTokenHandler(std::shared_ptr<ILogger> logger)
{
    return std::make_shared<TokenHandlerManager>(std::move(logger));
}

} // namespace app::server
