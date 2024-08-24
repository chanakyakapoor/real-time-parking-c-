#include "../floorPlan.h"
#include <gtest/gtest.h>

TEST(TestConstructor, Parameterized)
{
    FloorPlan f(12, "test_floor", 4, 5);
    ASSERT_EQ(12, f.getFloorNumber());
    ASSERT_EQ("test_floor", f.getFloorName());
    ASSERT_EQ(4, f.getNumberOfNormalParkingSlots());
    ASSERT_EQ(5, f.getNumberOfDisabledParkingSlots());
    ASSERT_TRUE(f.getPtrToArrayOfNormalParkingSlots() != nullptr);
    ASSERT_TRUE(f.getPtrToArrayOfDisabledParkingSlots() != nullptr);
}