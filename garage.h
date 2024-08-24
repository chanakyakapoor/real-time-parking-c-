#ifndef GARAGE_H
#define GARAGE_H
#include <sqlite3.h>
#include <string>
using namespace std;

class FloorPlan;
class ParkingSlot;

class Garage
{
	private:
		int numberOfFloors;
		FloorPlan *ptrToArrayOfFloorPlans;
		sqlite3* dbase;

	public:
		Garage();
		Garage(string parkingMapFile, string dbName);
		~Garage();  
		bool allotParkingSlot(string vNumber, string lNumber, char norDis);
		bool vacateParkingSlot(string vNumber);
        bool searchVehicleHistory(string vNumber);
		void specificFloorAvailability(int fNumber, int& normalCount, int& disabledCount);
		void allFloorsAvailability(int& normalCount, int& disabledCount);
		void specificFloorOccupancy(int fNumber, int& normalCount, int& disabledCount);
		void allFloorsOccupancy(int& normalCount, int& disabledCount);
		void parkingFeesCollection(string strStartDate, string strEndDate, int& parkingCFeesAmount);
		void fineCollection(string strStartDate, string strEndDate, int& fineFeesAmount);
		int calculateParkingFine(time_t entryTime, time_t exitTime);
        void displayAllDatabaseEntries();
		void deleteDatabase();
};

#endif