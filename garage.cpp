#include "greathalt.h"
#include <cmath>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/error/en.h>

using namespace rapidjson;

//Constructor
Garage::Garage()
{
    numberOfFloors = 0;
    ptrToArrayOfFloorPlans = nullptr;
    dbase = nullptr;
}

//Overloading constructor
Garage::Garage(string parkingMapFile, string dbName)
{
    dbase = nullptr;
    //file opening using binary read mode
    FILE* fp = fopen(parkingMapFile.c_str(), "rb");
    if (!fp) {
        cerr << "Error: Unable to open parking map file." << endl;
        exit(1);
    }

    char readBuffer[65536];                                      //declare buffer size to MAX
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));      //read the file to buffer
    Document doc;
    doc.ParseStream(is);
    fclose(fp);
    //Validating the JSON file parsed correctly
    if (doc.HasParseError()) {
        cerr << "Error parsing JSON file: " << GetParseError_En(doc.GetParseError()) << " at " << doc.GetErrorOffset() << endl;
        exit(1);
    }
    //Check for FloorsData key(array) is present or not
    if (!doc.HasMember("FloorsData") || !doc["FloorsData"].IsArray()) {
        cerr << "Error: Invalid JSON format. 'FloorsData' not found or is not an array." << endl;
        exit(1);
    }
    //initialization of floors data
    const Value& floorsData = doc["FloorsData"];
    numberOfFloors = floorsData.Size();
    ptrToArrayOfFloorPlans = new FloorPlan[numberOfFloors];

    for (SizeType i = 0; i < floorsData.Size(); ++i) {
        const Value& floor = floorsData[i];
        if (!floor.HasMember("FloorNumber") || !floor.HasMember("FloorName") || 
            !floor.HasMember("NormalParkingSlots") || !floor.HasMember("DisabledParkingSlots")) {
            cerr << "Error: Invalid JSON format for floor data at index " << i << endl;
            exit(1);
        }

        int fnum = floor["FloorNumber"].GetInt();
        string fName = floor["FloorName"].GetString();
        int numSlots = floor["NormalParkingSlots"].GetInt();
        int numDisabledSlots = floor["DisabledParkingSlots"].GetInt();

        ptrToArrayOfFloorPlans[i] = FloorPlan(fnum, fName, numSlots, numDisabledSlots);
    }

    //Check the Database existance
    int rc = sqlite3_open(dbName.c_str(), &dbase);
    if (rc) {
        cerr << "Error opening SQLite database: " << sqlite3_errmsg(dbase) << endl;
        sqlite3_close(dbase);
        exit(1);
    }

    //to create the ParkingRecords table
    const char* createTableSQL = 
    "CREATE TABLE IF NOT EXISTS ParkingRecords ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "vehicleNumber TEXT,"
    "licenseNumber TEXT,"
    "entryDate TEXT,"
    "entryTime TEXT,"
    "exitDate TEXT,"
    "exitTime TEXT,"
    "PFees INTEGER,"
    "FFees INTEGER);";



    char* errMsg = nullptr;
    rc = sqlite3_exec(dbase, createTableSQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error creating table: " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(dbase);
        exit(1);
    }
}

// Delete the database (latest update)
void Garage::deleteDatabase() {
    sqlite3_close(dbase);
    if (remove("greathalt.db") == 0) {
        std::cout << "Database deleted successfully." << std::endl;
    } else {
        std::cerr << "Error deleting the database." << std::endl;
    }
}

//Deconstructutor
Garage::~Garage()
{
    delete[] ptrToArrayOfFloorPlans;
        if (dbase) {
            sqlite3_close(dbase);
        }
}

 
bool Garage::allotParkingSlot(string vNumber, string lNumber, char norDis)
{
   //to check if the vehicle is already parked
   stringstream checkQuery;
    checkQuery << "SELECT COUNT(*) FROM ParkingRecords WHERE vehicleNumber = '" << vNumber << "' AND exitTime IS NULL;";

    //to count the number of rows returned by the query
    int count = 0;
    auto checkCallback = [](void* data, int argc, char** argv, char** /*azColName*/) -> int {
        int* rowCount = static_cast<int*>(data);
        *rowCount = atoi(argv[0]);
        return 0;
    };

    char* errMsg = nullptr;
    int rc = sqlite3_exec(dbase, checkQuery.str().c_str(), checkCallback, &count, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    if (count > 0) {
        cout << "The vehicle with number " << vNumber << " is already parked." << endl;
        return false; 
    }

    //Get the time datas
    time_t current_time = time(nullptr);
    string strDate, strTime;
    converttmDateTimeTostrDatestrTime(current_time, strDate, strTime);

    ParkingSlot* parkingSlots = nullptr;
    int numSlots = 0;

    // Check if the slot type is Normal/Dis
    if (norDis == 'N') {
        for (int i = 0; i < numberOfFloors; ++i) {
            numSlots = ptrToArrayOfFloorPlans[i].getNumberOfNormalParkingSlots();
            parkingSlots = ptrToArrayOfFloorPlans[i].getPtrToArrayOfNormalParkingSlots();
            for (int j = 0; j < numSlots; ++j) {
                if (parkingSlots[j].getVehicleNumber().empty()) {
                    parkingSlots[j].setVehicleNumber(vNumber);
                    parkingSlots[j].setLicenseNumber(lNumber);
                    parkingSlots[j].setEntryTime(current_time);

                    stringstream insertQuery;
                    insertQuery << "INSERT INTO ParkingRecords (vehicleNumber, licenseNumber, entryDate, entryTime, PFees, FFees) VALUES ('"
                                << vNumber << "', '" << lNumber << "', '" << strDate << "', '" << strTime << "', NULL, NULL);";

                    int rc = sqlite3_exec(dbase, insertQuery.str().c_str(), nullptr, nullptr, &errMsg);
                    if (rc != SQLITE_OK) {
                        cerr << "SQL error: " << errMsg << endl;
                        sqlite3_free(errMsg);
                        return false;
                    }

                    cout << "Allotment details:" << endl;
                    cout << "Vehicle Number: " << vNumber << endl;
                    cout << "License Number: " << lNumber << endl;
                    cout << "Slot Type: Normal" << endl;
                    cout << "Floor Number: " << ptrToArrayOfFloorPlans[i].getFloorNumber() << endl;
                    cout << "Floor Name: " << ptrToArrayOfFloorPlans[i].getFloorName() << endl;
                    cout << "Slot Number: " << j << endl;
                    cout << "Entry Date: " << strDate << endl;
                    cout << "Entry Time: " << strTime << endl;
                    cout << "Normal slot allotment entry done in database!" << endl << endl;

                    return true;
                }
            }
        }
    } else if (norDis == 'D') {
        for (int i = 0; i < numberOfFloors; ++i) {
            numSlots = ptrToArrayOfFloorPlans[i].getNumberOfDisabledParkingSlots();
            parkingSlots = ptrToArrayOfFloorPlans[i].getPtrToArrayOfDisabledParkingSlots();
            for (int j = 0; j < numSlots; ++j) {
                if (parkingSlots[j].getVehicleNumber().empty()) {
                    parkingSlots[j].setVehicleNumber(vNumber);
                    parkingSlots[j].setLicenseNumber(lNumber);
                    parkingSlots[j].setEntryTime(current_time);

                    stringstream insertQuery;
                    insertQuery << "INSERT INTO ParkingRecords (vehicleNumber, licenseNumber, entryDate, entryTime, PFees, FFees) VALUES ('"
                                << vNumber << "', '" << lNumber << "', '" << strDate << "', '" << strTime << "', NULL, NULL);";

                    int rc = sqlite3_exec(dbase, insertQuery.str().c_str(), nullptr, nullptr, &errMsg);
                    if (rc != SQLITE_OK) {
                        cerr << "SQL error: " << errMsg << endl;
                        sqlite3_free(errMsg);
                        return false;
                    }

                    cout << "Allotment details:" << endl;
                    cout << "Vehicle Number: " << vNumber << endl;
                    cout << "License Number: " << lNumber << endl;
                    cout << "Slot Type: Disabled" << endl;
                    cout << "Floor Number: " << ptrToArrayOfFloorPlans[i].getFloorNumber() << endl;
                    cout << "Floor Name: " << ptrToArrayOfFloorPlans[i].getFloorName() << endl;
                    cout << "Slot Number: " << j << endl;
                    cout << "Entry Date: " << strDate << endl;
                    cout << "Entry Time: " << strTime << endl;
                    cout << "Disabled slot allotment entry done in database!" << endl << endl;

                    return true;
                }
            }
        }
    }
    return false;
}
// Vacate vehicle and prints the vehicle details
bool Garage::vacateParkingSlot(string vNumber)
{
   stringstream checkQuery;
    checkQuery << "SELECT COUNT(*) FROM ParkingRecords WHERE vehicleNumber = '" << vNumber << "' AND exitTime IS NULL;";

    int count = 0;
    auto checkCallback = [](void* data, int argc, char** argv, char** /*azColName*/) -> int {
        int* rowCount = static_cast<int*>(data);
        *rowCount = atoi(argv[0]);
        return 0;
    };

    char* errMsg = nullptr;
    int rc = sqlite3_exec(dbase, checkQuery.str().c_str(), checkCallback, &count, &errMsg);
   //Check the entered data format
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    //Vehicle not found
    if (count == 0) {
        cout << "Parking record not found for entered vehicle number: " << vNumber << "." << endl;
        return false; 
    }

    //To obtain timings data
    time_t current_time = time(nullptr);
    string strDate, strTime;
    converttmDateTimeTostrDatestrTime(current_time, strDate, strTime);

    ParkingSlot* parkingSlots = nullptr;
    int numSlots = 0;

    for (int i = 0; i < numberOfFloors; ++i) {
        numSlots = ptrToArrayOfFloorPlans[i].getNumberOfNormalParkingSlots();
        parkingSlots = ptrToArrayOfFloorPlans[i].getPtrToArrayOfNormalParkingSlots();
        for (int j = 0; j < numSlots; ++j) {
            if (parkingSlots[j].getVehicleNumber() == vNumber) {

                time_t entryTime = parkingSlots[j].getEntryTime();
                string strEntryDate, strEntryTime;
                converttmDateTimeTostrDatestrTime(entryTime, strEntryDate, strEntryTime);
                int fFees = calculateParkingFine(entryTime, current_time);
                stringstream updateQuery;
                updateQuery << "UPDATE ParkingRecords SET exitDate = '" << strDate << "', exitTime = '" << strTime << "', PFees = 100, FFees = " << fFees << " WHERE vehicleNumber = '" << vNumber << "' AND exitTime IS NULL;";

                rc = sqlite3_exec(dbase, updateQuery.str().c_str(), nullptr, nullptr, &errMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL error: " << errMsg << endl;
                    sqlite3_free(errMsg);
                    return false;
                }

                cout << "Vehicle is already parked!" << endl;
                cout << "Vehicle Number: " << vNumber << endl;
                cout << "License Number: " << parkingSlots[j].getLicenseNumber() << endl;
                cout << "Floor Number: " << ptrToArrayOfFloorPlans[i].getFloorNumber() << endl;
                cout << "Floor Name: " << ptrToArrayOfFloorPlans[i].getFloorName() << endl;
                cout << "Slot Number: " << j << endl;
                cout << "Entry Date: " << strEntryDate << endl;
                cout << "Entry Time: " << strEntryTime << endl;
                cout << "Exit Date: " << strDate << endl;
                cout << "Exit Time: " << strTime << endl;
                cout << "Parking Fees: 100" << endl;
                cout << "Fine: " << fFees << endl << endl;

                parkingSlots[j].setVehicleNumber("");
                parkingSlots[j].setLicenseNumber("");
                parkingSlots[j].setExitTime(current_time);

                return true; // Slot vacated successfully
            }
        }

        numSlots = ptrToArrayOfFloorPlans[i].getNumberOfDisabledParkingSlots();
        parkingSlots = ptrToArrayOfFloorPlans[i].getPtrToArrayOfDisabledParkingSlots();
        for (int j = 0; j < numSlots; ++j) {
            if (parkingSlots[j].getVehicleNumber() == vNumber) {

                time_t entryTime = parkingSlots[j].getEntryTime();
                string strEntryDate, strEntryTime;
                converttmDateTimeTostrDatestrTime(entryTime, strEntryDate, strEntryTime);
                int fFees = calculateParkingFine(entryTime, current_time);

                stringstream updateQuery;
                updateQuery << "UPDATE ParkingRecords SET exitDate = '" << strDate << "', exitTime = '" << strTime << "', PFees = 100, FFees = " << fFees << " WHERE vehicleNumber = '" << vNumber << "' AND exitTime IS NULL;";

                rc = sqlite3_exec(dbase, updateQuery.str().c_str(), nullptr, nullptr, &errMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL error: " << errMsg << endl;
                    sqlite3_free(errMsg);
                    return false;
                }

                cout << "Vehicle is already parked!" << endl;
                cout << "Vehicle Number: " << vNumber << endl;
                cout << "License Number: " << parkingSlots[j].getLicenseNumber() << endl;
                cout << "Floor Number: " << ptrToArrayOfFloorPlans[i].getFloorNumber() << endl;
                cout << "Floor Name: " << ptrToArrayOfFloorPlans[i].getFloorName() << endl;
                cout << "Slot Number: " << j << endl;
                cout << "Entry Date: " << strEntryDate << endl;
                cout << "Entry Time: " << strEntryTime << endl;
                cout << "Exit Date: " << strDate << endl;
                cout << "Exit Time: " << strTime << endl;
                cout << "Parking Fees: 100" << endl;
                cout << "Fine Fees: " << fFees << endl << endl;

                parkingSlots[j].setVehicleNumber("");
                parkingSlots[j].setLicenseNumber("");
                parkingSlots[j].setExitTime(current_time);
                return true; 
            }
        }
    }

    cout << "Parking record not found for entered vehicle number: " << vNumber << "." << endl;
    return false; 
}

//Prints the vehcle history
bool Garage::searchVehicleHistory(string vNumber)
{
    stringstream query;
    query << "SELECT id, vehicleNumber, licenseNumber, entryDate, entryTime, exitDate, exitTime, PFees, FFees "
          << "FROM ParkingRecords WHERE vehicleNumber = '" << vNumber << "';";

    int count = 0; // To count the number of records found

    cout << left << setw(4) << "ID" << " " 
         << left << setw(20) << "Veh Num" << " " 
         << left << setw(20) << "Lic Num" << " " 
         << left << setw(10) << "En Date" << " " 
         << left << setw(10) << "En Time" << " " 
         << left << setw(10) << "Ex Date" << " " 
         << left << setw(10) << "Ex Time" << " " 
         << left << setw(6) << "P Fees" << " " 
         << left << setw(6) << "F Fees" << endl;


    auto callback = [](void* data, int argc, char** argv, char** /*azColName*/) -> int {
        int* rowCount = static_cast<int*>(data);
        (*rowCount)++;
        cout << left << setw(4) << (argv[0] ? argv[0] : "NULL") << " " 
         << left << setw(15) << (argv[1] ? argv[1] : "NULL") << " " 
         << left << setw(15) << (argv[2] ? argv[2] : "NULL") << " " 
         << left << setw(6) << (argv[3] ? argv[3] : "NULL") << " " 
         << left << setw(6) << (argv[4] ? argv[4] : "NULL") << " " 
         << left << setw(6) << (argv[5] ? argv[5] : "NULL") << " " 
         << left << setw(6) << (argv[6] ? argv[6] : "NULL") << " " 
         << left << setw(4) << (argv[7] ? argv[7] : "NULL") << " " 
         << left << setw(4) << (argv[8] ? argv[8] : "NULL") << endl;
        return 0;
    };

    char* errMsg = nullptr;
    int rc = sqlite3_exec(dbase, query.str().c_str(), callback, &count, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl << endl;
        sqlite3_free(errMsg);
        return false;
    } else if (count == 0) {
        cout << "No history found for the entered vehicle number: " << vNumber << "." << endl << endl;
        return false;
    } else {
        cout << "Vehicle history for " << vNumber << " retrieved successfully." << endl << endl;
        return true;
    }
}

//Displays specific floor slot details
void Garage::specificFloorAvailability(int fNumber, int& normalCount, int& disabledCount)
{
    normalCount = 0;
    disabledCount = 0;

    if (fNumber >= 0 && fNumber < numberOfFloors) {
        FloorPlan& floor = ptrToArrayOfFloorPlans[fNumber];
        //Check normal slots data
        ParkingSlot* normalSlots = floor.getPtrToArrayOfNormalParkingSlots();
        int numNormalSlots = floor.getNumberOfNormalParkingSlots();
        for (int i = 0; i < numNormalSlots; ++i) {
            if (normalSlots[i].getVehicleNumber().empty()) {
                ++normalCount;
            }
        }
        //Check disabled slots data
        ParkingSlot* disabledSlots = floor.getPtrToArrayOfDisabledParkingSlots();
        int numDisabledSlots = floor.getNumberOfDisabledParkingSlots();
        for (int i = 0; i < numDisabledSlots; ++i) {
            if (disabledSlots[i].getVehicleNumber().empty()) {
                ++disabledCount;
            }
        }

        cout << "Floor Number: " << fNumber << endl;
        cout << "Floor Name: " << floor.getFloorName() << endl;
        cout << "Number of Normal Slots: " << normalCount << endl;
        cout << "Number of Disabled Slots: " << disabledCount << endl << endl;
    } else {

        cerr << "Error: Floor number " << fNumber << " is not available." << endl << endl;
    }
}

//Displays all floors slot details
void Garage::allFloorsAvailability(int& normalCount, int& disabledCount)
{
    normalCount = 0;
    disabledCount = 0;

    for (int fNumber = 0; fNumber < numberOfFloors; ++fNumber) {
        int floorNormalCount = 0;
        int floorDisabledCount = 0;

        specificFloorAvailability(fNumber, floorNormalCount, floorDisabledCount);

        normalCount += floorNormalCount;
        disabledCount += floorDisabledCount;
    }

    cout << "Total availability across all floors:" << endl;
    cout << "Total Normal Slots: " << normalCount << endl;
    cout << "Total Disabled Slots: " << disabledCount << endl << endl;
}

//Displays specific floor occupancy details
void Garage::specificFloorOccupancy(int fNumber, int& normalCount, int& disabledCount)
{
    normalCount = 0;
    disabledCount = 0;

    if (fNumber >= 0 && fNumber < numberOfFloors) {
        FloorPlan& floor = ptrToArrayOfFloorPlans[fNumber];

        ParkingSlot* normalSlots = floor.getPtrToArrayOfNormalParkingSlots();
        int numNormalSlots = floor.getNumberOfNormalParkingSlots();
        cout << "Floor Number: " << fNumber << endl;
        cout << "Floor Name: " << floor.getFloorName() << endl;
        cout << "Normal Parking Slots:" << endl << endl;
        for (int i = 0; i < numNormalSlots; ++i) {
            if (!normalSlots[i].getVehicleNumber().empty()) {
                cout << "Slot Number: " << i << endl;
                cout << "Vehicle Number: " << normalSlots[i].getVehicleNumber() << endl;
                cout << "License Number: " << normalSlots[i].getLicenseNumber() << endl << endl;
                ++normalCount;
            }
            else {
                cout << "Slot Number: " << i << endl;
                cout << "Vehicle Number: " << "Available" << endl;
                cout << "License Number: " << "Available" << endl << endl;
            }
        }

        ParkingSlot* disabledSlots = floor.getPtrToArrayOfDisabledParkingSlots();
        int numDisabledSlots = floor.getNumberOfDisabledParkingSlots();
        cout << "Disabled Parking Slots:" << endl << endl;
        for (int i = 0; i < numDisabledSlots; ++i) {
            if (!disabledSlots[i].getVehicleNumber().empty()) {
                cout << "Slot Number: " << i << endl;
                cout << "Vehicle Number: " << disabledSlots[i].getVehicleNumber() << endl;
                cout << "License Number: " << disabledSlots[i].getLicenseNumber() << endl << endl;
                ++disabledCount;
            }
            else {
                cout << "Slot Number: " << i << endl;
                cout << "Vehicle Number: " << "Available" << endl;
                cout << "License Number: " << "Available" << endl << endl;
            }
        }

    } else {
        cerr << "Error: Floor number " << fNumber << " not available." << endl << endl;
    }
}

//Displays all floors occupancy details
void Garage::allFloorsOccupancy(int& normalCount, int& disabledCount)
{
    normalCount = 0;
    disabledCount = 0;

    for (int fNumber = 0; fNumber < numberOfFloors; ++fNumber) {
        int floorNormalCount = 0;
        int floorDisabledCount = 0;

        specificFloorOccupancy(fNumber, floorNormalCount, floorDisabledCount);

        normalCount += floorNormalCount;
        disabledCount += floorDisabledCount;
    }

    cout << "Total occupancy across all floors:" << endl;
    cout << "Total Occupied Normal Slots: " << normalCount << endl;
    cout << "Total Occupied Disabled Slots: " << disabledCount << endl << endl;
}

//fees collcetion data
void Garage::parkingFeesCollection(string strStartDate, string strEndDate, int& parkingFeesAmount)
{
    parkingFeesAmount = 0;

    stringstream query;
    query << "SELECT SUM(PFees) FROM ParkingRecords WHERE exitDate IS NOT NULL "
          << "AND exitDate >= '" << strStartDate << "' "
          << "AND exitDate <= '" << strEndDate << "';";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbase, query.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(dbase) << endl;
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        parkingFeesAmount += sqlite3_column_int(stmt, 0);
    } else {
        cerr << "Failed to retrieve parking fees data." << endl;
    }

    sqlite3_finalize(stmt);

    cout << "Total parking fees collected from " << strStartDate << " to " << strEndDate << ": Rs. " << parkingFeesAmount << endl << endl;
}


//Calculate fine if exceeded more than 3hours
int Garage::calculateParkingFine(time_t enTime, time_t exTime)
{
    double durationSeconds = difftime(exTime, enTime);

    const int maxAllowedDurationSeconds = 10800; 

    const int fineRateWithinGracePeriod = 0; 
    const int fineRatePerHour = 50; 

    if (durationSeconds <= maxAllowedDurationSeconds) {
        return fineRateWithinGracePeriod; 
    } else {
        double hoursParked = (durationSeconds - maxAllowedDurationSeconds) / 3600.0;

        int hoursParkedCeil = static_cast<int>(ceil(hoursParked));

        int fineAmount = hoursParkedCeil * fineRatePerHour;

        return fineAmount;
    }
}

//Give the total fine collected Data
void Garage::fineCollection(string strStartDate, string strEndDate, int& fineAmount)
{
    fineAmount = 0;

    stringstream query;
    query << "SELECT SUM(FFees) FROM ParkingRecords WHERE exitDate IS NOT NULL "
          << "AND exitDate >= '" << strStartDate << "' "
          << "AND exitDate <= '" << strEndDate << "';";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbase, query.str().c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << sqlite3_errmsg(dbase) << endl;
        return;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        fineAmount = sqlite3_column_int(stmt, 0);
    } else {
        cerr << "Failed to retrieve fine data." << endl;
    }

    sqlite3_finalize(stmt);

    cout << "Total fines collected from " << strStartDate << " to " << strEndDate << ": Rs. " << fineAmount << endl;
}

//To display all the database values
void Garage::displayAllDatabaseEntries()
{
    stringstream query;
    query << "SELECT id, vehicleNumber, licenseNumber, entryDate, entryTime, exitDate, exitTime, PFees, FFees "
          << "FROM ParkingRecords;";

    int count = 0; 

    cout << left << setw(2) << "ID" << " " 
         << left << setw(15) << "Veh Num" << " " 
         << left << setw(15) << "Lic Num" << " " 
         << left << setw(6) << "En Date" << " " 
         << left << setw(6) << "En Time" << " " 
         << left << setw(6) << "Ex Date" << " " 
         << left << setw(6) << "Ex Time" << " " 
         << left << setw(4) << "P Fees" << " " 
         << left << setw(4) << "F Fees" << endl;

    auto callback = [](void* data, int argc, char** argv, char** /*azColName*/) -> int {
        int* rowCount = static_cast<int*>(data);
        (*rowCount)++;
        cout << left << setw(4) << (argv[0] ? argv[0] : "NULL") << " " 
         << left << setw(15) << (argv[1] ? argv[1] : "NULL") << " " 
         << left << setw(15) << (argv[2] ? argv[2] : "NULL") << " " 
         << left << setw(6) << (argv[3] ? argv[3] : "NULL") << " " 
         << left << setw(6) << (argv[4] ? argv[4] : "NULL") << " " 
         << left << setw(6) << (argv[5] ? argv[5] : "NULL") << " " 
         << left << setw(6) << (argv[6] ? argv[6] : "NULL") << " " 
         << left << setw(4) << (argv[7] ? argv[7] : "NULL") << " " 
         << left << setw(4) << (argv[8] ? argv[8] : "NULL") << endl << endl;
        return 0;
    };

    char* errMsg = nullptr;
    int rc = sqlite3_exec(dbase, query.str().c_str(), callback, &count, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else if (count == 0) {
        cout << "Records not found in the database." << endl << endl;
    } else {
        cout << "Database records retrieved successfully." << endl << endl;
    }

}

