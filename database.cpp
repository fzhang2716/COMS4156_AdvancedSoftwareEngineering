// Sets up the server
#include <iostream>
#include <stdlib.h>
#include <string>
#include "data_management.hpp"
#include "crow.h"

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


std::string GetTestTable(){
    // database connection
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    driver = sql::mysql::get_mysql_driver_instance();
    // connect IP adress, username, password
    con = driver->connect("tcp://34.150.169.58", "admin", "debugteam"); 

    sql::Statement* stmt;
    sql::ResultSet* res;

    std::string data = "";
    stmt = con->createStatement();
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

    delete con;

    return data;
}

