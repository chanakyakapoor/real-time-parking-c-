#include "../parkingSlot.h"
#include <gtest/gtest.h>

TEST(TestConstructor, Parameterized)
{
    ParkingSlot p("test_vnumber", "test_lnumber", 1691865320, 1691865880);
    ASSERT_EQ("test_vnumber", p.getVehicleNumber());
    ASSERT_EQ("test_lnumber", p.getLicenseNumber());
    ASSERT_EQ(1691865320, p.getEntryTime());
    ASSERT_EQ(1691865880, p.getExitTime());
}