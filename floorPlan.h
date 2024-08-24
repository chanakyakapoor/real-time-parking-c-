#ifndef FLOORPLAN_H
#define FLOORPLAN_H
#include <string>
using namespace std;

class ParkingSlot; 

class FloorPlan
{
	private:
		int floorNumber;
		string floorName;
	    int numberOfNormalParkingSlots;
		int numberOfDisabledParkingSlots;
        ParkingSlot *ptrToArrayOfNormalParkingSlots;
        ParkingSlot *ptrToArrayOfDisabledParkingSlots;
	public:
		FloorPlan();
		FloorPlan(int fnum, string fName, int numSlots, int numDisabledSlots);
		int getFloorNumber();
		string getFloorName();
	    int getNumberOfNormalParkingSlots();
		int getNumberOfDisabledParkingSlots();
		ParkingSlot* getPtrToArrayOfNormalParkingSlots();
		ParkingSlot* getPtrToArrayOfDisabledParkingSlots();
};

#endif