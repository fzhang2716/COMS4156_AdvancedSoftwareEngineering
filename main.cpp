/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include "./Crow/include/crow.h"
#include "./data_management.hpp"
#include <jwt-cpp/jwt.h>

DataManagementService dataservice;

struct JwtMiddleware : crow::ILocalMiddleware {
    struct context {
        int companyId;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx){
        std::string authHeader = req.get_header_value("Authorization");
        if(authHeader.find("Bearer ") != 0){
            res.code = 401; //Unauthorized
            res.write("JWT token not found \n");
            res.end();
            return;
        }

        std::string jwtToken = authHeader.substr(7);

        int id = dataservice.verifyJwtToken(jwtToken);
        if( id == -1){
            res.code = 401; //Unauthorized
            res.write("Invalid token \n");
            res.end();
        } else{
            ctx.companyId = id;
        }

    }

    void after_handle(crow::request& req, crow::response& res, context& ctx){
    }
};

int main() {
    crow::App<JwtMiddleware> app;

    /**
     * Get a company's information
     * Example request: http://localhost:3000/company?username=company1&password=pwd
    */
    CROW_ROUTE(app, "/company")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&] (const crow::request& req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.getCompanyInfo(req, res, companyId);
    });

    // Post Method: collect company information and add to database
    CROW_ROUTE(app, "/addCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.addCompany(req, res);
    });

    //Post Method: request a new access token
    CROW_ROUTE(app, "/recoverCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.recoverCompany(req, res);
    });

    // Post Method: collect member information and add to database
    CROW_ROUTE(app, "/addMember")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.addMember(req, res, companyId);
    });

    // Post Method: collect subscription information and add to database
    CROW_ROUTE(app, "/addSubscription")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.addSubscription(req, res, companyId);
    });

    app.port(3000).multithreaded().run();
}
