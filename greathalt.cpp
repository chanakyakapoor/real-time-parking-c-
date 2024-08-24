#include "greathalt.h"

class FloorPlan;
class ParkingSlot;

void pressAnyKeyToContinue() {
    std::cout << "Press enter key to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//Display Menu Options
void printMenu(UserType userType) {
    switch (userType) {
        case UserType::admin:
            cout << "1. Allocate Slot" << endl;
            cout << "2. Vacate Slot" << endl;
            cout << "3. Search Vehicle History" << endl;
            cout << "4. Floor Specific Availability" << endl;
            cout << "5. All Floor Availability" << endl;
            cout << "6. Floor Specific Occupancy" << endl;
            cout << "7. All Floor Occupancy" << endl;
            cout << "8. Parking Fees Collection" << endl;
            cout << "9. Fine Collection" << endl;
            cout << "10. Display All Database Entries" << endl;
            cout << "0. Exit" << endl << endl;
            break;
            
        case UserType::user:
            cout << "1. Allocate Slot" << endl;
            cout << "2. Vacate Slot" << endl;
            cout << "3. Search Vehicle History" << endl;
            cout << "4. Floor Specific Availability" << endl;
            cout << "5. All Floor Availability" << endl;
            cout << "6. Floor Specific Occupancy" << endl;
            cout << "7. All Floor Occupancy" << endl;
            cout << "0. Exit" << endl << endl;
            break;
            
        case UserType::invalid:
            cout << "Invalid login credentials. Try again." << endl;
            break;
            
        default:
            cerr << "Unexpected user type encountered." << endl;
            break;
    }
}

int main() {

    Garage g("ParkingLotData.json", "greathalt.db");        //Create Database

    while (true) {
        cout << "Welcome to GreatHalt!" << endl;
        
        UserType actualUserType = authenticate("UserData.json");        //Authenticate the user credentials
        while(actualUserType == invalid){                               //if invalid ask again to enter
            cout<<"Invalid login credentials. Try again."<<endl;
            actualUserType = authenticate("UserData.json");
        }
        main_menu:
        cout << endl;
        printMenu(actualUserType);
        int option;
        cout << "Enter your choice: ";
        cin >> option;
        
        cin.ignore();  
        cout << endl;

        switch (option) {
            case 1: {
                // Allocate Slot
                string vNumber, lNumber;
                char norDis;
                
                cout << "Enter Vehicle Number: ";
                getline(cin, vNumber);
                
                cout << "Enter License Number: ";
                getline(cin, lNumber);
                
                cout << "Normal / Disabled (N/D): ";
                cin >> norDis;
                
                bool allocated = g.allotParkingSlot(vNumber, lNumber, norDis);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 2:{
                // Vacate Slot: 
                string vNumber;
                
                cout << "Enter Vehicle Number: ";
                getline(cin, vNumber);

                bool vacated = g.vacateParkingSlot(vNumber);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 3: {
                // Search Vehicle History
                string vNumber;
                
                cout << "Enter Vehicle Number: ";
                getline(cin, vNumber);
                
                bool found = g.searchVehicleHistory(vNumber);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 4:{
                // Floor Specific Availability: Implement this if needed
                int fNumber;
                int normalCount = 0;
                int disabledCount = 0;

                cout << "Enter floor no.: ";
                cin >> fNumber;
                
                g.specificFloorAvailability( fNumber,  normalCount,  disabledCount);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 5:{
                // All Floor Availability: 
                int normalCount = 0;
                int disabledCount = 0;

                g.allFloorsAvailability( normalCount,  disabledCount);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 6:{
                // Floor Specific Occupancy: 
                int fNumber;
                int normalCount = 0;
                int disabledCount = 0;

                cout << "Enter floor no.: ";
                cin >> fNumber;

                g.specificFloorOccupancy( fNumber,  normalCount,  disabledCount);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 7:{
                // All Floor Occupancy: 
                int normalCount = 0;
                int disabledCount = 0;

                g.allFloorsOccupancy( normalCount, disabledCount);

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 8:{
                if (actualUserType == UserType::admin) {
                    // Parking Fees Collection: 
                    string strStartDate, strEndDate;
                        int parkingCFeesAmount = 0;

                        cout << "Enter start date (DD/MM/YY): ";
                        cin >> strStartDate;
                        cout << "Enter end date (DD/MM/YY): ";
                        cin >> strEndDate;

                        g.parkingFeesCollection(strStartDate, strEndDate, parkingCFeesAmount);
                    } 
                else {
                    cout << "Unauthorized access. Only admins can access this function.Please login as admin" << endl;
                }

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 9:{
                if (actualUserType == UserType::admin) {
                    // Fine Collection: 
                    string strStartDate, strEndDate;
                        int fineFeesAmount = 0;
                        cout << "Enter entry date (DD/MM/YY): ";
                        cin >> strStartDate;
                        cout << "Enter exit date (DD/MM/YY): ";
                        cin >> strEndDate;

                        g.fineCollection( strStartDate,  strEndDate,  fineFeesAmount);

                } else {
                    cout << "Unauthorized access. Pls login as admin to access." << endl;
                }

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 10:{
                if (actualUserType == UserType::admin) {
                    // Display All Database Entries
                    g.displayAllDatabaseEntries();
                } else {
                    cout << "Unauthorized access. Pls login as admin to access." << endl;
                }

                pressAnyKeyToContinue();
                goto main_menu;
            }
            case 0:{
                cout << "Exiting..." << endl;
                g.deleteDatabase(); //Deleteing the database
                return 0;
            }
            default:{
                cout << "Invalid option. Please choose again." << endl << endl;
                goto main_menu;
            }
        }
    }

    return 0;
}