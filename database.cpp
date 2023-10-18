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

sql::Connection* DBConnect(){
    // database connection
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* conn;

    driver = sql::mysql::get_mysql_driver_instance();
    // connect IP adress, username, password
    conn = driver->connect("tcp://34.150.169.58", "admin", "debugteam"); 

    return conn;
}

void DBDisConnect(sql::Connection* conn){
    delete conn;
}

/**
 * If user is authorized, return its company_id
 * otherwise, return -1
*/
int isUserAuthenticated(const crow::request& req, crow::response& res, sql::Connection* conn) {
    // Extract username and password from the request.
    std::string username = req.url_params.get("username");
    std::string password = req.url_params.get("password");


    // Query your MySQL database to validate the credentials.
    // If the credentials are valid, return true; otherwise, return false.

    // Example of a query to check user credentials:
    try{
        sql::Statement* stmt = conn->createStatement();
        sql::ResultSet* result = stmt->executeQuery("SELECT * FROM service.company_table WHERE company_name = '" + username + "' AND hash_pwd = '" + password + "'");

        if (result->next()) {
            int companyId = result->getInt("company_id");
            std::cout << "Authorized company id is " << companyId << std::endl;
            delete result;
            delete stmt;
            res.code = 200;
            res.write("Authentication success");
            return companyId;
        }

        // If credentials are not valid, return false and deny access.
        delete result;
        delete stmt;
        res.code = 401; // Unauthorized
        res.write("Authentication failed");
        return -1;
    } catch (sql::SQLException& e ) {
        res.code = 500;
        res.write("Database Error: " + std::string(e.what()));
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

