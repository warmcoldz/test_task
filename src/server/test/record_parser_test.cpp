#include <gtest/gtest.h>
#include <source/record_parser.h>

namespace app::server {

constexpr size_t HeaderSize{ sizeof(uint16_t) + sizeof(uint8_t) };

TEST(RecordParserTest, ProcessGreetings)
{
    const std::vector<uint8_t> greetings{
        0, 9, /*RecordLength*/
        0, /*Type*/

        1, /*Version*/
        5, /*ClientId Length*/
        'h', 'e', 'l', 'l', 'o', /*ClientId*/
        0, 15 /*Tokens*/
    };

    auto greetingsRange{ core::MakeConstBlobRange(greetings) };

    RecordParser parser;
    std::optional<Record> record{ parser.ProcessData(greetingsRange) };

    ASSERT_TRUE(greetingsRange.empty());
    ASSERT_TRUE(record.has_value());
    ASSERT_EQ(protocol::RecordType::Greetings, record->GetType());
    ASSERT_TRUE(std::equal(
        std::begin(greetings) + HeaderSize,
        std::end(greetings),
        record->GetPayload().begin()
    ));
}

TEST(RecordParserTest, ProcessToken)
{
    const std::vector<uint8_t> token{
        0, 5, /*RecordLength*/
        2, /*Type*/

        4, /*Token Length*/
        'a', 'b', 'c', 'd' /*Token*/
    };

    auto tokenRange{ core::MakeConstBlobRange(token) };

    RecordParser parser;
    std::optional<Record> record{ parser.ProcessData(tokenRange) };

    ASSERT_TRUE(tokenRange.empty());
    ASSERT_TRUE(record.has_value());
    ASSERT_EQ(protocol::RecordType::Token, record->GetType());
    ASSERT_TRUE(std::equal(
        std::begin(token) + HeaderSize,
        std::end(token),
        record->GetPayload().begin()
    ));
}

TEST(RecordParserTest, ProcessTokens)
{
    const std::vector<uint8_t> token1{
        0, 5, /*RecordLength*/
        2, /*Type*/

        4, /*Token Length*/
        'a', 'b', 'c', 'd', /*Token*/
    };

    const std::vector<uint8_t> token2{
        0, 3, /*RecordLength*/
        2, /*Type*/

        2, /*Token Length*/
        'a', 'b', /*Token*/
    };

    const std::vector<uint8_t> token3{
        0, 7, /*RecordLength*/
        2, /*Type*/

        6, /*Token Length*/
        'a', 'b', 'h', 'h', 'h', 'h' /*Token*/
    };

    for (const auto& token : { token1, token2, token3 })
    {
        auto tokenRange{ core::MakeConstBlobRange(token) };

        RecordParser parser;
        std::optional<Record> record{ parser.ProcessData(tokenRange) };

        ASSERT_TRUE(tokenRange.empty());
        ASSERT_TRUE(record.has_value());
        ASSERT_EQ(protocol::RecordType::Token, record->GetType());
        ASSERT_TRUE(std::equal(
            std::begin(token) + HeaderSize,
            std::end(token),
            record->GetPayload().begin()
        ));
    }
}

} // namespace app::server
