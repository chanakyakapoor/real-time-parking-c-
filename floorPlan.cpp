#include "greathalt.h"
using namespace std;


// Default constructor for the FloorPlan class
FloorPlan::FloorPlan()
{
    floorNumber = 0;
    floorName = "";
    numberOfNormalParkingSlots = 0;
    numberOfDisabledParkingSlots = 0;
    ptrToArrayOfNormalParkingSlots = nullptr;
    ptrToArrayOfDisabledParkingSlots = nullptr;
}

// Parameterized constructor for the FloorPlan class
FloorPlan::FloorPlan(int fnum, string fName, int numSlots, int numDisabledSlots)
{
    floorNumber = fnum;
    floorName = fName;
    numberOfNormalParkingSlots = numSlots;
    numberOfDisabledParkingSlots = numDisabledSlots;
    ptrToArrayOfNormalParkingSlots = new ParkingSlot[numSlots];                  // Allocate memory for parking Slots Objects
    ptrToArrayOfDisabledParkingSlots = new ParkingSlot[numDisabledSlots];
}

// Get for floor Number
int FloorPlan::getFloorNumber()
{
    return floorNumber;
}

// Get for floor Number
string FloorPlan::getFloorName()
{
    return floorName;
}

//Get normal Parking slots
int FloorPlan::getNumberOfNormalParkingSlots()
{
    return numberOfNormalParkingSlots;
}

//Get disabled Parking slots
int FloorPlan::getNumberOfDisabledParkingSlots()
{
    return numberOfDisabledParkingSlots;
}

//Pointer to  normal parking slot array
ParkingSlot * FloorPlan::getPtrToArrayOfNormalParkingSlots()
{
    return ptrToArrayOfNormalParkingSlots;
}

//Pointer to Disabled parking slot array
ParkingSlot * FloorPlan::getPtrToArrayOfDisabledParkingSlots()
{
    return ptrToArrayOfDisabledParkingSlots;
}