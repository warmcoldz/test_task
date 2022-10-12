#include <protocol/record_builder.h>
#include <gtest/gtest.h>

namespace app::protocol {

TEST(RecordBuilderTest, Greetings)
{
    RecordBuilder recordBuilder;

    {
        const std::vector<uint8_t> actualGreetings{ recordBuilder.MakeGreetingsRecord("hello", 15) };
        const std::vector<uint8_t> expectedGreetings{
            0, 9, /*RecordLength*/
            0, /*Type*/

            1, /*Version*/
            5, /*ClientId Length*/
            'h', 'e', 'l', 'l', 'o', /*ClientId*/
            0, 15 /*Tokens*/
        };

        ASSERT_EQ(expectedGreetings, actualGreetings);
    }

    {
        const std::vector<uint8_t> actualGreetings{ recordBuilder.MakeGreetingsRecord("clt", 256) };
        const std::vector<uint8_t> expectedGreetings{
            0, 7, /*RecordLength*/
            0, /*Type*/

            1, /*Version*/
            3, /*ClientId Length*/
            'c', 'l', 't',  /*ClientId*/
            1, 0 /*Tokens*/
        };

        ASSERT_EQ(expectedGreetings, actualGreetings);
    }
}

TEST(RecordBuilderTest, Ready)
{
    RecordBuilder recordBuilder;
    const std::vector<uint8_t> actualReady{ recordBuilder.MakeReadyRecord() };
    const std::vector<uint8_t> expectedReady{
        0, 0, /*RecordLength*/
        1, /*Type*/
    };

    ASSERT_EQ(expectedReady, actualReady);
}

TEST(RecordBuilderTest, Token)
{
    RecordBuilder recordBuilder;
    const std::vector<uint8_t> actualToken{ recordBuilder.MakeTokenRecord("abcd") };
    const std::vector<uint8_t> expectedToken{
        0, 5, /*RecordLength*/
        2, /*Type*/

        4, /*Token Length*/
        'a', 'b', 'c', 'd' /*Token*/
    };

    ASSERT_EQ(expectedToken, actualToken);
}

TEST(RecordBuilderTest, LargeRecord)
{
    static constexpr size_t TokenSize{ 255 };
    const std::string token(TokenSize, 'a');

    RecordBuilder recordBuilder;
    const std::vector<uint8_t> actualToken{ recordBuilder.MakeTokenRecord(token) };

    std::vector<uint8_t> expectedToken{
        1 , 0, /*RecordLength*/
        2, /*Type*/

        TokenSize, /*Token Length*/
    };
    expectedToken.insert(expectedToken.cend(), token.cbegin(), token.cend());

    ASSERT_EQ(expectedToken, actualToken);
}

} // namespace app::protocol
