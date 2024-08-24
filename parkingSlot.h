#ifndef PARKINGSLOT_H
#define PARKINGSLOT_H
#include <string>
using namespace std;

class ParkingSlot
{ 
	private:
		string vehicleNumber;
		string licenseNumber;
		time_t entryTime;
		time_t exitTime;
	public:
		ParkingSlot();
		ParkingSlot(string vNumber, string lNumber, time_t enTime, time_t exTime);
		string getVehicleNumber();
		string getLicenseNumber();
		time_t getEntryTime();
		time_t getExitTime();
		void setVehicleNumber(string vNumber);
		void setLicenseNumber(string lNumber);
		void setEntryTime(time_t t);
		void setExitTime(time_t t);
};  
#endif