#include <core/network_reader.h>
#include <gtest/gtest.h>

namespace app::core {

TEST(NetworkReaderTest, ReadInts)
{
    std::vector<uint8_t> values{ 
        0x01,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x01, 0x02,
        0x01, 0x02, 0x03, 0x04,
        0x22, 0x11,
    };

    auto valuesRange{ MakeConstBlobRange(values) };

    NetworkReader reader{ valuesRange };
    ASSERT_EQ(0x01, reader.Read<uint8_t>());
    ASSERT_EQ(0x0102030405060708, reader.Read<uint64_t>());
    ASSERT_EQ(0x0102, reader.Read<uint16_t>());
    ASSERT_EQ(0x01020304, reader.Read<uint32_t>());
    ASSERT_EQ(0x2211, reader.Read<uint16_t>());
}

TEST(NetworkReaderTest, ReadStrings)
{
    const std::vector<uint8_t> values{ 
        'h', 'e', 'l', 'l', 'o',
        'w', 'o', 'r', 'l', 'd', '!'
    };

    auto valuesRange{ MakeConstBlobRange(values) };
    NetworkReader reader{ valuesRange };
    ASSERT_EQ("hello", reader.ReadAsString(5));
    ASSERT_EQ("world!", reader.ReadAsString(6));
}

TEST(NetworkReaderTest, ReadMixed)
{
    const std::vector<uint8_t> values{ 
        'h', 'e', 'l', 'l', 'o',
        0xaa, 0xbb, 0xcc, 0xdd,
        'w', 'o', 'r', 'l', 'd', '!'
    };

    auto valuesRange{ MakeConstBlobRange(values) };
    NetworkReader reader{ valuesRange };
    ASSERT_EQ("hello", reader.ReadAsString(5));
    ASSERT_EQ(0xaabbccdd, reader.Read<uint32_t>());
    ASSERT_EQ("world!", reader.ReadAsString(6));
}

TEST(NetworkReaderTest, ReadAfterEnd)
{
    const std::vector<uint8_t> values{ 'h', 'e', 'l', 'l', 'o' };

    auto valuesRange{ MakeConstBlobRange(values) };
    NetworkReader reader{ valuesRange };
    ASSERT_EQ("hello", reader.ReadAsString(5));
    ASSERT_THROW(reader.ReadAsString(111), std::runtime_error);
}

} // namespace app::core
