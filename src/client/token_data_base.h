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
        std::string line;

        const bool lineRead{ std::getline(m_file, line) };
        if (lineRead && !line.empty())
        {
            m_readTokens.push_back(std::move(line));
            return m_readTokens.back();
        }

        CHECK(!m_readTokens.empty(), "No tokens provided");
        return m_readTokens[std::rand() % m_readTokens.size()];
    }

private:
    std::ifstream m_file;
    std::vector<std::string> m_readTokens;
};

} // namespace app::client
