#include "greathalt.h"
using namespace std;

ParkingSlot::ParkingSlot()
{
    entryTime = 0;
    exitTime = 0;
}
//Allot parking slot
ParkingSlot::ParkingSlot(string vNumber, string lNumber, time_t enTime, time_t exTime)
{
    vehicleNumber = vNumber;
    licenseNumber = lNumber;
    entryTime = enTime;
    exitTime = exTime;
}
//get vehicle number
string ParkingSlot::getVehicleNumber()
{
    return  vehicleNumber;
}
//get license number
string ParkingSlot::getLicenseNumber()
{
    return licenseNumber;
}
//Get entry time of the vehicle
time_t ParkingSlot::getEntryTime()
{
    return entryTime;
}
// Get exit time of the vehicle
time_t ParkingSlot::getExitTime()
{
    return exitTime;
}
//Set vehicle number
void ParkingSlot::setVehicleNumber(string vNumber)
{
    vehicleNumber = vNumber;
}
//Set the license number
void ParkingSlot::setLicenseNumber(string lNumber)
{
    licenseNumber = lNumber;
}
//Set entry time of vehicle
void ParkingSlot::setEntryTime(time_t t)
{
    entryTime = t;
}
//Set exit time of vehicle
void ParkingSlot::setExitTime(time_t t)
{
    exitTime = t;
}
