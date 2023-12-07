/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */

#define CATCH_CONFIG_RUNNER
#include <jwt-cpp/jwt.h>
#include <json/json.h>
#include <crow.h>
#include <./catch2/catch.hpp>
#include "./data_management.hpp"
#include "cpp-httplib/httplib.h"
#include <thread>  //NOLINT

DataManagementService dataservice;

/**
 * This class is used for test setup
*/
class TestServer {
 public:
    static void StartServer() {
        if (!instance) {
            instance = new TestServer();
            instance->run();
        }
    }

    static void StopServer() {
        if (instance) {
            instance->stop();
            delete instance;
            instance = nullptr;
        }
    }

 private:
        TestServer() {}
        void run() {}
        void stop() {}

    static TestServer* instance;
};

TestServer* TestServer::instance = nullptr;

class ServerFixture {
 public:
        static httplib::Headers headers;
        ServerFixture() {
        }
};

httplib::Headers ServerFixture::headers = {
    {"Authorization", "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzIwMzA2ODgsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMCJ9.DZGXi-CvQeQOhwK9HB4FYWZMjZr-J914FT5_UaFC_44"}
};

// Global setup
int main(int argc, char* argv[]) {
    TestServer::StartServer();

    int result = Catch::Session().run(argc, argv);

    TestServer::StopServer();

    return result;
}

TEST_CASE_METHOD(ServerFixture, "/company test", "[routes][company]") {
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

TEST_CASE_METHOD(ServerFixture, "/company/addCompany test", "[routes][addCompany]") {
    httplib::Client client("localhost", 3000);
    std::string jsonData = R"({"email": "fzhang0821@gmail","company_name":"testCompany"})";

    auto response = client.Post("/company/addCompany", headers, jsonData, "application/json");

    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["msg"] = "Add Company Success";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(targetJson["msg"] == responseJson["msg"]);
    if ((response->status == 200) &&(targetJson["msg"] == responseJson["msg"])) {
        std::string sqlCommend = "DELETE FROM service.company_table WHERE (email = 'fzhang0821@gmail' and company_name = 'testCompany')";
        std::string deleteData = R"({"sqlCommed": ")" + sqlCommend + R"("})";
        client.Delete("/admin/deleteRecord", headers, deleteData, "application/json");
    }
}

TEST_CASE_METHOD(ServerFixture, "/member/addMember test", "[routes][addMember]") {
    // std::cout << "Test running" << std::endl;
    httplib::Client client("localhost", 3000);
    std::string jsonData = R"({"first_name": "mike", "last_name": "tyson", "email": "tyson@gmail.com", "password": "123", "phone_number": "1234"})";

    auto response = client.Post("/member/addMember", headers, jsonData, "application/json");

    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["msg"] = "Add Member Success";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(responseJson == targetJson);
    if ((response->status == 200) &&(responseJson == targetJson)) {
        std::string sqlCommand = "DELETE FROM service.member_table WHERE (email = 'tyson@gmail.com') and (first_name = 'mike') and (last_name = 'tyson') and (hash_pwd = '123') and (phone_number = '1234')";  // NOLINT
        std::string deleteData = R"({"sqlCommed": ")" + sqlCommand + R"("})";
        client.Delete("/admin/deleteRecord", headers, deleteData, "application/json");
    }
}

TEST_CASE_METHOD(ServerFixture, "/subscription/addSubscription test", "[routes][addSubscription]") {
    httplib::Client client("localhost", 3000);
    std::string jsonData = R"({
        "member_email": "m2@gmail.com",
        "subscription_name": "espn",
        "subscription_type": "Free",
        "subscription_status": "active",
        "next_due_date": "2023-12-31",
        "start_date": "2023-01-01",
        "billing_info": "info"
    })";

    auto response = client.Post("/subscription/addSubscription", headers, jsonData, "application/json");

    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["msg"] = "Add Subscription Success";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(responseJson == targetJson);
    if ((response->status == 200)&& (responseJson == targetJson)) {
        std::string sqlCommand = "SET SQL_SAFE_UPDATES = 0; DELETE FROM service.subscription_table WHERE(member_email = 'm2@gmail.com') AND(subscription_name = 'espn'); SET SQL_SAFE_UPDATES = 1;";
        std::string deleteData = R"({"sqlCommed": ")" + sqlCommand + R"("})";
        client.Delete("/admin/deleteRecord", headers, deleteData, "application/json");
    }
}


TEST_CASE_METHOD(ServerFixture, "/admin/member/changeMemberInfo test", "[routes][changeMemberInfo]") {
    httplib::Client client("localhost", 3000);
    std::string jsonData = R"({
        "email": "m2@gmail.com",
        "first_name": "new_name",
        "last_name": "new_name",
        "phone_number": "123213"
    })";

    auto response = client.Patch("/admin/member/changeMemberInfo", headers, jsonData, "application/json");
    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["msg"] = "Update success";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(responseJson == targetJson);
}



TEST_CASE_METHOD(ServerFixture, "/admin/member/removeMember/<string> and addmemeber test", "[routes][addSubscription]") {
    httplib::Client client("localhost", 3000);

    std::string jsonData = R"({"first_name": "mike", "last_name": "tyson", "email": "tyson@gmail.com", "password": "123", "phone_number": "1234"})";

    auto response = client.Post("/member/addMember", headers, jsonData, "application/json");

    REQUIRE(response->status == 200);

    std::string email = "tyson@gmail.com";
    std::string requestUrl = "/admin/member/removeMember/" + email;

    auto deleteResponse = client.Delete(requestUrl.c_str(), headers, "", "application/json");
    REQUIRE(deleteResponse->status == 204);

    auto deleteAgainResponse = client.Delete(requestUrl.c_str(), headers, "", "application/json");
    REQUIRE(deleteAgainResponse->status == 400);
}

// need login, do it later
// TEST_CASE_METHOD(ServerFixture, "/member/changeMemberInfo test", "[routes][changeMemberInfo]") {
//     httplib::Client client("localhost", 3000);
//     std::string jsonData = R"({
//         "email": "m2@gmail.com",
//         "first_name": "new_name",
//         "last_name": "new_name",
//         "phone_number": "123213"
//     })";
//     auto response = client.Patch("/member/changeMemberInfo", headers, jsonData, "application/json");
//     REQUIRE(response->status == 200);

//     Json::Value targetJson;
//     targetJson["msg"] = "Update success";

//     Json::CharReaderBuilder reader;
//     Json::Value responseJson;
//     std::istringstream responseBody(response->body);
//     Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
//     REQUIRE(responseJson == targetJson);
// }

TEST_CASE_METHOD(ServerFixture, "/subscription/updateSubscription test", "[routes][updateSubscription]") {
    httplib::Client client("localhost", 3000);
    std::string login_jsonData = R"({
        "email": "m2@gmail.com",
        "password": "123"})";
    auto login_response = client.Post("/member/login", headers, login_jsonData, "application/json");
    REQUIRE(login_response->status == 200);
    std::string cookies = login_response->get_header_value("Set-Cookie");


    std::string jsonData = R"({
        "subscription_id": "9",
        "subscription_status": "canceled",
        "billing_info": "0111 credit card"})";

    headers.insert(std::make_pair("Cookie", cookies));
    auto response = client.Patch("/subscription/updateSubscription", headers, jsonData, "application/json");
    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["msg"] = "Update Subscription Success";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(responseJson == targetJson);
}

TEST_CASE_METHOD(ServerFixture, "/company/getExpiringSubscriptionByTime test", "[routes][updateSubscription]") {
    httplib::Client client("localhost", 3000);
    std::string url = "/company/getExpiringSubscriptionByTime?subscription_name=hulu&days=100";
    auto response = client.Get(url, headers);
    REQUIRE(response->status == 200);

    Json::Value targetJson;
    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(responseJson["0"] == "m2@gmail.com");
    REQUIRE(responseJson["number"] == "1");
}


TEST_CASE_METHOD(ServerFixture, "/company/sendReminder test", "[routes][sendReminder]") {
    httplib::Client client("localhost", 3000);
    std::string jsonData = R"({
        "number": "1",
        "target_time": "2023-11-28 20:07:46",
        "0": "hz2716@columbia.edu"})";

    auto response = client.Post("/company/sendReminder", headers, jsonData, "application/json");
    REQUIRE(response->status == 200);

    Json::Value targetJson;
    targetJson["msg"] = "Send successfully";

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::istringstream responseBody(response->body);
    Json::parseFromStream(reader, responseBody, &responseJson, nullptr);
    REQUIRE(responseJson == targetJson);
}
