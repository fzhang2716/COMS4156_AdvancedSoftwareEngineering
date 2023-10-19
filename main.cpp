/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include "./Crow/include/crow.h"
#include "./data_management.hpp"

int main() {
    crow::SimpleApp app;
    DataManagementService dataservice;

    /**
     * Get a company's information
     * Example request: http://localhost:3000/company?username=company1&password=pwd
    */
    CROW_ROUTE(app, "/company")
    .methods(crow::HTTPMethod::GET)
    ([&] (const crow::request& req, crow::response& res){
        dataservice.getCompanyInfo(req, res);
    });

    // Post Method: collect company information and add to database
    CROW_ROUTE(app, "/addCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.addCompany(req, res);
    });

    // Post Method: collect member information and add to database
    CROW_ROUTE(app, "/addMember")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.addMember(req, res);
    });

    // Post Method: collect subscription information and add to database
    CROW_ROUTE(app, "/addSubscription")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.addSubscription(req, res);
    });

    app.port(3000).multithreaded().run();
}
