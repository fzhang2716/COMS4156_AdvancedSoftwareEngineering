// Sets up the server
#include <iostream>
#include <stdlib.h>
#include <string>
#include "data_management.hpp"
#include "crow.h"

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

/**
 * Connect to the database
 * Return a pointer to the sql::Connection
*/
sql::Connection* DBConnect(){
    // Database connection
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* conn;

    driver = sql::mysql::get_mysql_driver_instance();
    // Connect IP adress, username, password
    conn = driver->connect("tcp://34.150.169.58", "admin", "debugteam"); 

    return conn;
}

void DBDisConnect(sql::Connection* conn){
    delete conn;
}


/**
 * Query MySQL database to validate the credentials in the request parameters
 * If the credentials are valid, return the corresponding company id; otherwise, return -1.
*/
int isUserAuthenticated(const crow::request& req, crow::response& res, sql::Connection* conn) {
    // Extract username and password from the request.
    std::string username = req.url_params.get("username");
    std::string password = req.url_params.get("password");

    // Try query the databse
    try{
        sql::Statement* stmt = conn->createStatement();
        sql::ResultSet* result = stmt->executeQuery("SELECT * FROM service.company_table WHERE company_name = '" + username + "' AND hash_pwd = '" + password + "'");

        if (result->next()) {
            int companyId = result->getInt("company_id");
            delete result;
            delete stmt;
            res.code = 200;
            res.write("Authentication success \n");
            return companyId;
        }

        // If credentials are not valid, return -1 and deny access.
        delete result;
        delete stmt;
        res.code = 401; // Unauthorized
        res.write("Authentication failed \n");
        res.end();
    } catch (sql::SQLException& e ) { // Catch any SQL errors
        res.code = 500;
        res.write("Database Error: " + std::string(e.what()) + "\n");
        res.end();
    }

    return -1;
}


std::string GetTestTable(sql::Connection* conn){
    sql::Statement* stmt;
    sql::ResultSet* res;

    std::string data = "";
    stmt = conn->createStatement();
    res = stmt->executeQuery("SELECT * FROM test_schema.test_user_tbl");

    while(res->next()){
        data += "Id: ";
        data += res->getString("id");
        data +=  ", User Name: ";
        data +=  res->getString("userName");
        data += "; ";
    }

    delete res;
    delete stmt;

    return data;
}

