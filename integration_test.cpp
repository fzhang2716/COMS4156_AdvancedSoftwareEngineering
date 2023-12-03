/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */

#define CATCH_CONFIG_MAIN
#include <./catch2/catch.hpp>
#include "./data_management.hpp"
#include "cpp-httplib/httplib.h"
#include <jwt-cpp/jwt.h>
#include <json/json.h>
#include <crow.h>
#include <thread>

DataManagementService dataservice;

std::string jwtToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzIwMzA2ODgsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMCJ9.DZGXi-CvQeQOhwK9HB4FYWZMjZr-J914FT5_UaFC_44";  //JWT token （hardcode an example for demo）
httplib::Headers headers = {
    {"Authorization", "Bearer " + jwtToken}
};

crow::SimpleApp app;

std::atomic<bool> serverRunning(false);
std::thread serverThread([]() {
    app.port(8080).run();
    serverRunning = false;
});

void startServer() {
    serverRunning = true;
    serverThread.detach();
}

void stopServer() {
    if (serverRunning) {
        serverRunning = false;
    }
}

TEST_CASE("/company test", "[routes]" ) {
    startServer();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    httplib::Client client("localhost", 3000);

    auto response = client.Get("/company", headers);
    REQUIRE(response->status == 200);
    //std::string response1 = response->body;
    //response1.erase(std::remove(response1.begin(), response1.end(), '\n'), response1.end());
    
    Json::Value targetJson;
    targetJson["company_id"] = "20";
    targetJson["company_name"] = "frank";
    targetJson["email"] = "hz2716@columbia.edu";
    
    // Parse request body as JSON
    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);

    // Compare the two JSON objects
    REQUIRE(responseJson == targetJson);
    stopServer();
}