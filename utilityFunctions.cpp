#include "utilityFunctions.h"
#include "greathalt.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <openssl/sha.h>
#include <iostream>


using namespace rapidjson;
string sha256(string str);

// Function to compute SHA256 hash of a string
string sha256(string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Function to parse JSON data from a file and return a RapidJSON Document
Document parseJsonFromFile(const string& filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    char readBuffer[65536]; 
    FileReadStream is(fp, readBuffer, sizeof(readBuffer)); 

    Document doc;
    doc.ParseStream(is);

    fclose(fp); 
    return doc; 
}

// Function to authenticate user by verifying login credentials
enum UserType authenticate(string dataFile) {

   Document doc = parseJsonFromFile(dataFile);

    string loginId;
    string password;

    cout << "Please enter your login ID: ";
    cin >> loginId;
    cout << "Please enter your password: ";
    cin >> password;

    string hashedPassword = sha256(password);
    const Value& usersArray = doc["Users"];
    string role;
    // Check if the current entry is a valid user object with necessary fields
    for (SizeType i = 0; i < usersArray.Size(); i++) {
        const Value& users = usersArray[i];
        if (users.IsObject() && users.HasMember("loginname") && users.HasMember("pwSHA256ChkSum")) {
            if (users["loginname"].IsString() && users["pwSHA256ChkSum"].IsString()) {
                if (users["loginname"].GetString() == loginId && users["pwSHA256ChkSum"].GetString() == hashedPassword) {
                    if (users.HasMember("role") && users["role"].IsString()) {
                        role = users["role"].GetString();
                        if (role == "admin") {
                            return admin;
                        } else {
                            return user;
                        }
                }
            }
        }
    }
    } 

    return invalid;
}


