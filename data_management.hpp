// Sets up the server
#include <iostream>
#include <stdlib.h>
#include <string>
#include "crow.h"

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


class DataManagementService {
public:
    void collectData(const std::vector<std::string>& rawData);
    void updateData();
    void authenticateUser(const crow::request& req, crow::response& res);
};

/**
 * Connect to the database
 * Return a pointer to the sql::Connection
*/
sql::Connection* DBConnect();
void DBDisConnect(sql::Connection* conn);
int isUserAuthenticated(const crow::request& req, crow::response& res, sql::Connection* conn);