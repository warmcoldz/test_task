#include <core/endian.h>
#include <gtest/gtest.h>

namespace app::core {

TEST(HostToNetworkTest, Simple)
{
    ASSERT_EQ(0x11, HostToNetwork<uint8_t>(0x11));
    ASSERT_EQ(0x1122, HostToNetwork<uint16_t>(0x2211));
    ASSERT_EQ(0x11223344, HostToNetwork<uint32_t>(0x44332211));
    ASSERT_EQ(0x1122334455667788, HostToNetwork<uint64_t>(0x8877665544332211));
}

TEST(NetworkToHostTest, Simple)
{
    ASSERT_EQ(0x11, NetworkToHost<uint8_t>(0x11));
    ASSERT_EQ(0x1122, NetworkToHost<uint16_t>(0x2211));
    ASSERT_EQ(0x11223344, NetworkToHost<uint32_t>(0x44332211));
    ASSERT_EQ(0x1122334455667788, NetworkToHost<uint64_t>(0x8877665544332211));
}

} // namespace app::core
