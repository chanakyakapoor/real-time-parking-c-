g++ giventests/garageGivenTests.cpp garage.cpp floorPlan.cpp parkingSlot.cpp utilityFunctions.cpp dateTimeFunctions.cpp -o garageGivenTests -lgtest -lgtest_main -pthread -std=c++14 -lsqlite3 -lssl -lcrypto
./garageGivenTests

g++ giventests/utilityFunctionsGivenTests.cpp utilityFunctions.cpp -o utilityFunctionsGivenTests -lgtest -lgtest_main -pthread -std=c++14 -lsqlite3 -lssl -lcrypto
./utilityFunctionsGivenTests

g++ giventests/floorPlanGivenTests.cpp floorPlan.cpp parkingSlot.cpp -o floorPlanGivenTests -lgtest -lgtest_main -pthread -std=c++14 -lsqlite3 -lssl -lcrypto
./floorPlanGivenTests

g++ giventests/parkingSlotGivenTests.cpp parkingSlot.cpp -o parkingSlotGivenTests -lgtest -lgtest_main -pthread -std=c++14 -lsqlite3 -lssl -lcrypto
./parkingSlotGivenTests

rm ./garageGivenTests giventests/smallgreathalt.db ./utilityFunctionsGivenTests ./floorPlanGivenTests ./parkingSlotGivenTests