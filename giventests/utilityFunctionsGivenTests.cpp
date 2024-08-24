#include "../utilityFunctions.h"
#include <gtest/gtest.h>

TEST(TestAuthentication, RegularUser)
{
    istringstream input("john\nAnalgin100!\n");
    streambuf* origCinBuf = cin.rdbuf(input.rdbuf());
    testing::internal::CaptureStdout();
    UserType actualUserType = authenticate("UserData.json");
    cin.rdbuf(origCinBuf);
    string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(user, actualUserType);
}

TEST(TestAuthentication, AdminUser)
{
    istringstream input("doe\nCrocin100!\n");
    streambuf* origCinBuf = cin.rdbuf(input.rdbuf());
    testing::internal::CaptureStdout();
    UserType actualUserType = authenticate("UserData.json");
    cin.rdbuf(origCinBuf);
    string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(admin, actualUserType);
}

TEST(TestAuthentication, InvalidUser)
{
    istringstream input("david\nEdwin100!\n");
    streambuf* origCinBuf = cin.rdbuf(input.rdbuf());
    testing::internal::CaptureStdout();
    UserType actualUserType = authenticate("UserData.json");
    cin.rdbuf(origCinBuf);
    string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(invalid, actualUserType);
}
