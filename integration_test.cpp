/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */

#define CATCH_CONFIG_MAIN
#include <./catch2/catch.hpp>
#include "./data_management.hpp"
#include "cpp-httplib/httplib.h"
#include <jwt-cpp/jwt.h>
#include <crow.h>
#include <thread>

// Function to set up Crow routes for real testing
void setupRoutes(crow::SimpleApp& app) {
    // Implement the actual handlers for real routes here
    // For example:
    CROW_ROUTE(app, "/company")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    methods(crow::HTTPMethod::GET)
    ([&] (const crow::request& req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.getCompanyInfo(req, res, companyId);
    });

    // Add other real routes for testing here
    // ...
}


TEST_CASE("Real routes reliability test", "[real-routes]") {
    crow::SimpleApp app;
    setupRoutes(app); // Set up real routes

    std::thread serverThread([&app]() { app.port(8080).run(); }); // Start server in a separate thread

    // Perform API calls to real routes using cpp-httplib client
    httplib::Client client("localhost", 8080);

    // Example GET request to /company route
    auto response = client.Get("/company");
    std::string response1;
    if (response && response->status == 200) {
        response1 = response->body;
    }

    // Add assertions to check the reliability of your real routes
    // Ensure response1 contains the expected data based on your API implementation
    REQUIRE(response1 == "Expected real changeMemberInfo response");

    serverThread.detach(); // Detach the server thread
}