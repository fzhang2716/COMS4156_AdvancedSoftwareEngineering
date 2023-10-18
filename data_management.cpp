#include "data_management.hpp"

void DataManagementService::collectData(const std::vector<std::string>& rawData) {
    // Logic to collect data
}

void DataManagementService::updateData() {
    // Logic to update and process new data
}

void DataManagementService::authenticateUser(const crow::request& req, crow::response& res) {
    sql::Connection* conn = DBConnect();
        // User Authentication
        int companyId = isUserAuthenticated(req, res, conn);

        if(companyId != -1){
            sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM service.company_table WHERE company_id = ?");
            prep_stmt->setInt(1,companyId);

            try{
                sql::ResultSet* queryResult = prep_stmt->executeQuery();
                if(queryResult->rowsCount() > 0){
                    std::string companyData= ""; 
                    while( queryResult->next()){
                        companyData += "Company ID: " + queryResult->getString("company_id") + "; ";
                        companyData += "Company Name: " + queryResult->getString("company_name") + "; ";
                        companyData += "Company email: " + queryResult->getString("email") + " \n";
                    }
                    res.code = 200;
                    res.write("Result: " + companyData);
                    res.end();
                }else{
                    res.code = 200;
                    res.write("No Query Found");
                    res.end();
                }
                
                delete queryResult;
                delete prep_stmt;
            } catch (const sql::SQLException& e){
                res.code = 500;
                res.write("Database Error: " + std::string(e.what()) + "\n");
                res.end();
            }
        }
        DBDisConnect(conn);
}

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