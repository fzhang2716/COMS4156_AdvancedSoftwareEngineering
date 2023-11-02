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

#include <string>
#include <iostream>

#include "./Crow/include/crow.h"

class DataManagementService {
 public:
    const std::string secret_key = "4156debugteam";
 
    std::string generateJwtToken(int user_id);

    int verifyJwtToken(const std::string& token);
    
    /**
     * Check if the client is authorized
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * @param conn the database connection
     * @return the company_id of the client if authorized, or -1 if not
    */
    int isUserAuthenticated(const crow::request& req, crow::response& res,
                            sql::Connection* conn);
     /**
     * Get the client's company information
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * 
    */
    void getCompanyInfo(const crow::request& req, crow::response& res, int companyId);
    /**
     * Add a new client as company
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * 
    */
    void addCompany(const crow::request& req, crow::response& res);
    /**
     * Add a new member
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * 
    */
    void addMember(const crow::request& req, crow::response& res);
     /**
     * Add a new subscription
     * 
     * @param req the API request from client
     * @param res the response to be sent
     * 
    */
    void addSubscription(const crow::request& req, crow::response& res);
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
