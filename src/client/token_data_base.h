#pragma once

#include <core/exception.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace app::client {

class TokenDataBase
{
public:
    explicit TokenDataBase(const std::string& databasePath)
        : m_file{ databasePath }
    {
        CHECK(m_file, "Failed to open token file");
        std::srand(std::time(nullptr));
    }

    std::string GetArbitraryToken()
    {
        if (!EofReached())
        {
            std::string token;
            if (ReadTokenFromFile(token))
                return token;
        }
        
        return GetRandomTokenFromMemory();
    }

private:
    bool EofReached() const
    {
        return m_eofReached;
    }

    bool ReadTokenFromFile(std::string& token)
    {
        while (std::getline(m_file, token))
        {
            if (!token.empty())
            {
                m_readTokens.push_back(token);
                return true;
            }
        }

        m_eofReached = true;
        CHECK(!m_readTokens.empty(), "No tokens provided");
        return false;
    }

    std::string GetRandomTokenFromMemory() const
    {
        return m_readTokens[std::rand() % m_readTokens.size()];
    }

private:
    bool m_eofReached{ false };
    std::ifstream m_file;
    std::vector<std::string> m_readTokens;
};

} // namespace app::client
