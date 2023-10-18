#include "crow.h"
#include "data_management.hpp"

int main()
{
    crow::SimpleApp app;
    DataManagementService dataservice;

    CROW_ROUTE(app, "/test")
    ([]()
     {
        // result of a function
        return "somthing"; });

    /**
     * Get a company's information
     * Example request: http://localhost:3000/company?username=company1&password=pwd
    */

    CROW_ROUTE(app, "/company")
        .methods(crow::HTTPMethod::GET)
    ([&] (const crow::request& req, crow::response& res)
    {
        dataservice.getCompanyInfo(req, res);
    });

    //Post Method: get company information
    CROW_ROUTE(app, "/addCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res)
    {    
        dataservice.addCompany(req, res);
    });
    
    app.port(3000).multithreaded().run();
}