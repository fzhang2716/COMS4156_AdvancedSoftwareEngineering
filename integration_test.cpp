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

/**
 * This class is used for test setup
*/
class ServerFixture {
public:
    httplib::Headers headers;
   
    ServerFixture() { 
        headers.emplace("Authorization", jwtToken);
        //std::cout << "ServerFixture constructor called" << std::endl;
        startServer();
        // Optionally: Wait for the server to be ready before proceeding
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ~ServerFixture() {
        //std::cout << "ServerFixture destructor called" << std::endl; 
        stopServer();
    }
    

    void startServer() {
        //std::cout << "startServer called" << std::endl; 
        serverThread = std::thread([this]() {
            app.port(8080).multithreaded().run();
            serverRunning = false;
        });
        serverRunning = true;
        serverThread.detach();
    }

    void stopServer() {
        //std::cout << "stopServer called" << std::endl;
        app.stop();
        if (serverRunning) {
            serverRunning = false;
        }
    }

private:
    crow::SimpleApp app;
    std::atomic<bool> serverRunning{false};
    std::thread serverThread;
    std::string jwtToken ="Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzIwMzA2ODgsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMCJ9.DZGXi-CvQeQOhwK9HB4FYWZMjZr-J914FT5_UaFC_44";
};

TEST_CASE_METHOD(ServerFixture, "/company test", "[routes][company]") {
    //std::cout << "Test running" << std::endl;
    httplib::Client client("localhost", 3000);

    auto response = client.Get("/company", headers);
    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["company_id"] = "20";
    targetJson["company_name"] = "frank";
    targetJson["email"] = "hz2716@columbia.edu";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);

    REQUIRE(responseJson == targetJson);
}