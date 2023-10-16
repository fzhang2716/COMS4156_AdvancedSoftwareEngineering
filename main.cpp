// Sets up the server
#include <iostream>
#include <stdlib.h>
#include "data_management.hpp"
#include "crow.h"

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


int main() {
    //DataManagementService datamanage;
    crow::SimpleApp app; //define your crow application

    // //define your endpoint at the root directory
    // CROW_ROUTE(app, "/")([](){
    //     return "Hello world";
    // });

    // //set the port, set the app to run on multiple threads, and run the app
    // app.port(18080).multithreaded().run();

    // test database connection
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://34.150.169.58", "admin", "debugteam");

    sql::Statement* stmt;
    sql::ResultSet* res;

    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM test_schema.test_user_tbl");

    while(res->next()){
        std::cout << "Id: " << res->getString("id") << ", User Name: " << res->getString("userName") << std::endl;
    }

    delete res;
    delete stmt;

    delete con;

    return 0;
}