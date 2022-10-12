#include "record_type.h"
#include <core/checked_cast.h>
#include <core/algo.h>
#include <vector>
#include <string>

namespace app::protocol {

class RecordBuilder
{
public:
    std::vector<uint8_t> MakeGreetingsRecord(const std::string& clientId, uint32_t tokenCount)
    {
        return MakeRecord(protocol::RecordType::Greetings, MakeGreetingsPayload(clientId, tokenCount));
    }

    std::vector<uint8_t> MakeReadyRecord()
    {
        return MakeRecord(protocol::RecordType::Ready);
    }

    std::vector<uint8_t> MakeTokenRecord(const std::string& token)
    {
        return MakeRecord(protocol::RecordType::Token, MakeTokenPayload(token));
    }

private:
    std::vector<uint8_t> MakeGreetingsPayload(const std::string& clientId, uint16_t tokenCount)
    {
        constexpr uint8_t Version{ 1 };

        std::vector<uint8_t> payload;
        payload.push_back(Version);
        payload.push_back(core::CheckedStaticCast<uint8_t>(clientId.length()));
        core::Append(payload, clientId);
        core::AppendIntegerInNetworkOrder(payload, tokenCount);

        return payload;
    }

    std::vector<uint8_t> MakeTokenPayload(const std::string& token)
    {
        std::vector<uint8_t> payload;
        payload.push_back(core::CheckedStaticCast<uint8_t>(token.length()));
        core::Append(payload, token);

        return payload;
    }

    std::vector<uint8_t> MakeRecord(protocol::RecordType type, const std::vector<uint8_t>& payload = {})
    {
        std::vector<uint8_t> record;
        core::AppendIntegerInNetworkOrder<uint16_t>(record, payload.size());
        record.push_back(type);
        core::Append(record, payload);
        return record;
    }
};

} // namespace app::protocol
