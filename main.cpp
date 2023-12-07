/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include <jwt-cpp/jwt.h>
#include "./Crow/include/crow.h"
#include "crow/middlewares/session.h"
#include "./data_management.hpp"

DataManagementService dataservice;

struct JwtMiddleware : crow::ILocalMiddleware {
    struct context {
        int companyId;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        std::string authHeader = req.get_header_value("Authorization");
        if (authHeader.find("Bearer ") != 0) {
            res.code = 401;  // Unauthorized
            res.write("JWT token not found \n");
            res.end();
            return;
        }

        std::string jwtToken = authHeader.substr(7);

        int id = dataservice.verifyJwtToken(jwtToken);
        if (id == -1) {
            res.code = 401;  // Unauthorized
            res.write("Invalid token \n");
            res.end();
        } else {
            ctx.companyId = id;
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx){
    }
};

int main() {
    // Set up session with in memoery store
    using Session = crow::SessionMiddleware<crow::InMemoryStore>;
    // Register cookie parser
    crow::App<JwtMiddleware, crow::CookieParser, Session> app{Session{
        crow::CookieParser::Cookie(dataservice.generateSessionSecret()).max_age(24*60*60).path("/"),
        4,
        crow::InMemoryStore{}
    }};

    // For testing session keys
    CROW_ROUTE(app, "/")
    ([&](const crow::request& req) {
        auto& session = app.get_context<Session>(req);

        // get all currently present keys
        auto keys = session.keys();

        std::string out;
        for (const auto& key : keys)
            // .string(key) converts a value of any type to a string
            out += "<p> " + key + " = " + session.string(key) + "</p>";
        return out;
    });

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

    // Get Method: get all members of a company
    CROW_ROUTE(app, "/company/getMembers")
     .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.getCompanyMembers(req, res, companyId);
    });

    // Post Method: request a new access token
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

    // Post Method: member login
    CROW_ROUTE(app, "/member/login")
    .methods(crow::HTTPMethod::POST)
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    ([&](const crow::request& req, crow::response &res){
        auto& session = app.get_context<Session>(req);
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;

        std::string email = dataservice.memberLogin(req, res, companyId);
        std::cout << "email from login:" << email << std::endl;

        if (!email.empty()) {
            session.set("email", email);
            session.set("companyId", std::to_string(companyId));
            res.write("Login Successfully");
        }
        res.end();
    });

    // Post Method: member logout
    CROW_ROUTE(app, "/member/logout")
    .methods(crow::HTTPMethod::POST)
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    ([&](const crow::request& req, crow::response &res){
        auto& session = app.get_context<Session>(req);

        session.remove("email");
        session.remove("companyId");
        res.write("Logout Successfully");
        res.code = 200;
        res.end();
    });

    // Get Method: get member's profile (login cookies protected)
    CROW_ROUTE(app, "/member/profile")
    .methods(crow::HTTPMethod::GET)
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    ([&](const crow::request& req, crow::response &res){
        auto& session = app.get_context<Session>(req);
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;

        std::string email = session.get("email", "NA");
        if (email == "NA") {
            res.write("Auhorization Failed. Have not logged in.");
            res.code = 400;
            res.end();
        } else {
            dataservice.getMemberInfo(req, res, companyId, email);
        }
    });
    // Get Method: get member's profile (unprotected)
    CROW_ROUTE(app, "/admin/member/profile/<string>")
    .methods(crow::HTTPMethod::GET)
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    ([&](const crow::request& req, crow::response &res, std::string email){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.getMemberInfo(req, res, companyId, email);
    });

    // Delete Method: delete member
    CROW_ROUTE(app, "/admin/member/removeMember/<string>")
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
            auto& session = app.get_context<Session>(req);
            auto& ctx = app.get_context<JwtMiddleware>(req);
            int companyId = ctx.companyId;
            std::string email = session.get("email", "NA");
            if (email == "NA") {
                res.write("Auhorization Failed. Have not logged in.");
                res.code = 400;
                res.end();
            } else {
                dataservice.changeMemberInfo(req, res, companyId, email);
            }
    });
    // Patch Method: update member infomation
    CROW_ROUTE(app, "/admin/member/changeMemberInfo")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::PATCH)(
        [&](const crow::request &req, crow::response &res) {
            auto& ctx = app.get_context<JwtMiddleware>(req);
            int companyId = ctx.companyId;
            dataservice.changeMemberInfoAdmin(req, res, companyId);
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
       auto& session = app.get_context<Session>(req);
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        std::string email = session.get("email", "NA");
        if (email == "NA") {
            res.write("Auhorization Failed. Have not logged in.");
            res.code = 400;
            res.end();
        } else {
            dataservice.updateSubscription(req, res, companyId, email);
        }
    });

    // Patch Method: update the status of a given subscription as admin
    CROW_ROUTE(app, "/admin/subscription/updateSubscription")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::PATCH)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.updateSubscriptionAdmin(req, res, companyId);
    });

    // Patch Method: update the last_action and last_action_date fields of a given subscription
    CROW_ROUTE(app, "/subscription/updateSubscriptionAction")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::PATCH)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.updateSubscriptionAction(req, res, companyId);
    });

    // Get Method: view subscriptions of a company
    CROW_ROUTE(app, "/subscription/allSubscriptions")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.getCompanySubscriptions(req, res, companyId);
    });

    // Get Method: view subscriptions of a member with a company as admin
    CROW_ROUTE(app, "/admin/subscription/viewSubscriptions")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.viewSubscriptions(req, res, companyId, true, "");
    });

    // Get Method: view subscriptions of a member with a company as admin
    CROW_ROUTE(app, "/subscription/viewSubscriptions")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::GET)
    ([&](const crow::request &req, crow::response &res){
        auto& session = app.get_context<Session>(req);
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        std::string email = session.get("email", "NA");
        if (email == "NA") {
            res.write("Auhorization Failed. Have not logged in.");
            res.code = 400;
            res.end();
        } else {
           dataservice.viewSubscriptions(req, res, companyId, false, email);
        }
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
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.sendReminder(req, res, companyId);
    });

    // test Method for delete added record
    CROW_ROUTE(app, "/admin/deleteRecord")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::DELETE)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.deleteByString(req, res, companyId);
    });

    // POST Method: send email to company about the analysis of their member subscription durations
    CROW_ROUTE(app, "/company/analyzeSubDuration")
    .CROW_MIDDLEWARES(app, JwtMiddleware)
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        auto& ctx = app.get_context<JwtMiddleware>(req);
        int companyId = ctx.companyId;
        dataservice.analyzeSubDuration(req, res, companyId);
    });

    app.port(3000).multithreaded().run();
}
