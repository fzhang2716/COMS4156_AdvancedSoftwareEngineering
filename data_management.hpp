/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#ifndef DATA_MANAGEMENT_HPP_
#define DATA_MANAGEMENT_HPP_

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <stdlib.h>
#include <curl/curl.h>

#include <string>
#include <iostream>

#include "./Crow/include/crow.h"

class DataManagementService {
 public:
    const std::string secret_key = "4156debugteam";
    const std::string sendGrid_key = "";

    /**
     * Generate a random session secret for member login
    */
    std::string generateSessionSecret();
    /**
     * Generate a JWT token for the client
     * 
     * @param client_id the client's id
     * @return a string JWT token
    */
    std::string generateJwtToken(int client_id);
    /**
     * Verify the client's token
     * 
     * @param token the JWT token that client provides
     * @return the company_id of the client if authorized; otherwise, -1
    */
    int verifyJwtToken(const std::string& token);
    /**
     * Add a new client as company
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * 
    */
    void addCompany(const crow::request& req, crow::response& res);
     /**
     * Recover the client's JWT token by sending him an email
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * 
    */
    void recoverCompany(const crow::request& req, crow::response& res);
     /**
     * Get the client's company information
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void getCompanyInfo(const crow::request& req, crow::response& res, int companyId);
    /**
     * Change a company's information
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void changeCompany(const crow::request& req, crow::response& res, int companyId);
    /**
     * Get all members of a company
     *
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT 
    */
    void getCompanyMembers(const crow::request& req, crow::response& res, int companyId);
    /**
     * Add a new member
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void addMember(const crow::request& req, crow::response& res, int companyId);
    /**
     * Member Login
    */
    std::string memberLogin(const crow::request& req, crow::response& res, int companyId);
    /**
     * delete a existed memeber
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param deletedEmail the email of the member account to be deleted
     * 
    */
    void removeMember(const crow::request& req, crow::response& res, int companyId, std::string deleteEmail);
    /**
     * change infomation of an existing memeber as admin
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void changeMemberInfoAdmin(const crow::request& req, crow::response& res, int companyId);
    /**
     * change infomation of a member by himself
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * @param email the email get from the session after login
     * 
    */
    void changeMemberInfo(const crow::request& req, crow::response& res, int companyId, std::string email);
     /**
     * get infomation of a member
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * @param email the email get from the session after login
     * 
    */
    void getMemberInfo(const crow::request& req, crow::response& res, int companyId, std::string email);
     /**
     * Add a new subscription for its member
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void addSubscription(const crow::request& req, crow::response& res, int companyId);
    /**
     * change infomation of an existing subscription
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void updateSubscription(const crow::request& req, crow::response& res, int companyId, std::string email);
    /**
     * update subscription action
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void updateSubscriptionAction(const crow::request &req, crow::response &res, int companyId);
    /**
     * change infomation of an existing subscription as admin
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void updateSubscriptionAdmin(const crow::request& req, crow::response& res, int companyId);
    /**
     * Get all subscriptions of a company
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void getCompanySubscriptions(const crow::request& req, crow::response& res, int companyId);
    /**
     * view all subscriptions of a member with a company
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * @param email the member's email (is is Admin is true, no need to provide)
     * 
    */
    void viewSubscriptions(const crow::request& req, crow::response& res, int companyId, bool isAdmin, std::string email);


    /**
     * return a list of exipiring subscriptions
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void getExpiringSubscriptionByTime(const crow::request& req, crow::response& res, int companyId);

    /**
     * send reminder to a list of email
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     * 
    */
    void sendReminder(const crow::request& req, crow::response& res, int companyId);

   /**
    * analyze subscription durations of a company
    *
    * @param req the API request from client
    * @param res the response to be sent
    * @param companyId the companyId decoded from client's JWT
    * 
    */
    void analyzeSubDuration(const crow::request& req, crow::response& res, int companyId);

    /**
     * this is method is merely for removing added element in integration test
     * not reachable by any routes
     * @param req the API request from client
     * @param res the response to be sent
     * @param companyId the companyId decoded from client's JWT
     */
    void deleteByString(const crow::request &req, crow::response &res, int companyId);
};

/**
 * Connect to the database
 * 
 * @return a pointer to the sql::Connection
*/
sql::Connection* DBConnect();

/**
 * Disconnect the database
 * 
 * @param conn the database connection to be disconnected
*/
void DBDisConnect(sql::Connection* conn);

#endif  // DATA_MANAGEMENT_HPP_
