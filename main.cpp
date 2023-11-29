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
    CROW_ROUTE(app, "/company/addCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.addCompany(req, res);
    });

    // Patch Method: update a company's infomation
    CROW_ROUTE(app, "/company/changeCompany")
     .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::PATCH)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.changeCompany(req, res, companyId);
    });

    //Post Method: request a new access token
    CROW_ROUTE(app, "/recoverCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        dataservice.recoverCompany(req, res);
    });

    // Post Method: collect member information and add to database
    CROW_ROUTE(app, "/member/addMember")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.addMember(req, res, companyId);
    });

    // Delete Method: delete member
    CROW_ROUTE(app, "/member/removeMember/<string>")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods("DELETE"_method)(
        [&](const crow::request &req, crow::response &res, std::string deleteEmail) {
            auto& ctx = app.get_context<JwtMiddleware>(req);
            int companyId = ctx.companyId;
            dataservice.removeMember(req, res, companyId, deleteEmail);
        });

    // Patch Method: update member infomation
    CROW_ROUTE(app, "/member/changeMemberInfo")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::PATCH)(
        [&](const crow::request &req, crow::response &res) {
            auto& ctx = app.get_context<JwtMiddleware>(req);
            int companyId = ctx.companyId;
            dataservice.changeMemberInfo(req, res, companyId);
        });

    // Post Method: collect subscription information and add to database
    CROW_ROUTE(app, "/subscription/addSubscription")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.addSubscription(req, res, companyId);
    });

    // Patch Method: update the status of a given subscription
    CROW_ROUTE(app, "/subscription/updateSubscription")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::PATCH)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.updateSubscription(req, res, companyId);
    });

    // Get Method: get a list of email about expiring subscription
    CROW_ROUTE(app, "/company/getExpiringSubscriptionByTime")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.getExpiringSubscriptionByTime(req, res, companyId);
    });

    // Get Method: send email to a list of about expiring subscription
    CROW_ROUTE(app, "/company/sendReminder")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.sendReminder(req, res, companyId);
    });


    app.port(3000).multithreaded().run();
}
