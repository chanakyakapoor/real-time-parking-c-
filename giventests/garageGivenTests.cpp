#include "../garage.h"
#include "../utilityFunctions.h"
#include "../dateTimeFunctions.h"
#include <gtest/gtest.h>

TEST(TestGarageOperations, SmallParkingLot)
{
    Garage g("giventests/SmallParkingLotData.json", "giventests/smallgreathalt.db");
    time_t t;
    int normalCount, disabledCount;

    // Tests for before a vehicle is parked
    g.specificFloorAvailability(0, normalCount, disabledCount);
    ASSERT_EQ(1, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.specificFloorAvailability(4, normalCount, disabledCount);
    ASSERT_EQ(0, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.allFloorsAvailability(normalCount, disabledCount);
    ASSERT_EQ(2, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.specificFloorOccupancy(1, normalCount, disabledCount);
    ASSERT_EQ(0, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.specificFloorOccupancy(5, normalCount, disabledCount);
    ASSERT_EQ(0, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.allFloorsOccupancy(normalCount, disabledCount);
    ASSERT_EQ(0, normalCount);
    ASSERT_EQ(0, disabledCount);

    // Search for vehicle history
    bool history = g.searchVehicleHistory("test_vnumber");
    ASSERT_FALSE(history);

    // Allocate a normal parking slot to a vehicle
    bool test_vnumber_allot1 = g.allotParkingSlot("test_vnumberN1", "test_lnumberN1", 'N');
    ASSERT_TRUE(test_vnumber_allot1);

    // Try to allot a normal parking slot to an already parked vehicle
    test_vnumber_allot1 = g.allotParkingSlot("test_vnumberN1", "test_lnumberN1", 'N');
    ASSERT_FALSE(test_vnumber_allot1);

    // Allocate a normal parking slot to another vehicle
    bool test_vnumber_allot2 = g.allotParkingSlot("test_vnumberN2", "test_lnumberN2", 'N');
    ASSERT_TRUE(test_vnumber_allot2);

    // Try to allot a disable parking slot to a vehicle
    bool test_vnumber_disable_allot = g.allotParkingSlot("test_vnumberD1", "test_lnumberD1", 'D');
    ASSERT_FALSE(test_vnumber_disable_allot);

    // Check availability and occupancy after vehicles are parked 
    g.specificFloorAvailability(1, normalCount, disabledCount);
    ASSERT_EQ(0, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.specificFloorOccupancy(0, normalCount, disabledCount);
    ASSERT_EQ(1, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.allFloorsAvailability(normalCount, disabledCount);
    ASSERT_EQ(0, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.allFloorsOccupancy(normalCount, disabledCount);
    ASSERT_EQ(2, normalCount);
    ASSERT_EQ(0, disabledCount);

    // Vacate a parked vehicle
    bool test_vnumber_vacate1 = g.vacateParkingSlot("test_vnumberN1");
    ASSERT_TRUE(test_vnumber_vacate1);

    // Try to vacate an already vacated vehicle
    test_vnumber_vacate1 = g.vacateParkingSlot("test_vnumberN1");
    ASSERT_FALSE(test_vnumber_vacate1);

    // Try to vacate a vehicle that was never parked
    bool test_vnumber_disable_vacate = g.vacateParkingSlot("test_vnumberD1");
    ASSERT_FALSE(test_vnumber_disable_vacate);

    // Check availability and occupancy after one vehicle is removed
    g.specificFloorAvailability(0, normalCount, disabledCount);
    ASSERT_EQ(1, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.specificFloorOccupancy(1, normalCount, disabledCount);
    ASSERT_EQ(1, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.allFloorsAvailability(normalCount, disabledCount);
    ASSERT_EQ(1, normalCount);
    ASSERT_EQ(0, disabledCount);

    g.allFloorsOccupancy(normalCount, disabledCount);
    ASSERT_EQ(1, normalCount);
    ASSERT_EQ(0, disabledCount);

    history = g.searchVehicleHistory("test_vnumberN1");
    ASSERT_TRUE(history);

    time_t currentEpoch = time(nullptr);
    time_t startEpoch = currentEpoch - 86400;
    time_t endEpoch = currentEpoch + 86400;

    string startTime, endTime, startDate, endDate;
    converttmDateTimeTostrDatestrTime(startEpoch, startDate, startTime);
    converttmDateTimeTostrDatestrTime(endEpoch, endDate, endTime);
    
    int fees;
    g.parkingFeesCollection(startDate, endDate, fees);
    ASSERT_EQ(100, fees);

    int fine;
    g.fineCollection(startDate, endDate, fine);
    ASSERT_EQ(0, fine);

    fine = g.calculateParkingFine(startEpoch, endEpoch);
    ASSERT_EQ(2250, fine);

    // Re-parking an already vacated car in the garage
    test_vnumber_allot1 = g.allotParkingSlot("test_vnumberN1", "test_lnumberN1", 'N');
    ASSERT_TRUE(test_vnumber_allot1);

    // Vacating the parked car from the garage
    test_vnumber_vacate1 = g.vacateParkingSlot("test_vnumberN1");
    ASSERT_TRUE(test_vnumber_vacate1);
}