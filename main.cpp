#include "crow.h"
//#include "database.cpp"
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
    

    // CROW_ROUTE(app, "/json")
    // ([]
    //  {
    // sql::Connection* conn = DBConnect();
    // crow::json::wvalue x({{"message", GetTestTable(conn)}});
    // DBDisConnect(conn);
    // return x; });


    /**
     * Get a company's information
     * Example request: http://localhost:3000/company?username=company1&password=pwd
    */
    CROW_ROUTE(app, "/company")
        .methods(crow::HTTPMethod::GET)
    ([&] (const crow::request& req, crow::response& res)
    {
        dataservice.authenticateUser(req, res);
    });
    
    app.port(3000).multithreaded().run();
}