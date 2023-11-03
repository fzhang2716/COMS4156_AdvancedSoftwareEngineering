/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include "data_management.hpp"
#include "utils.hpp"
#include <jwt-cpp/jwt.h>
#include <exception>
#include <curl/curl.h>

using namespace std;

Query queryGenerator;

std::string DataManagementService::generateJwtToken(int user_id){
    // create a token valid for 1 year
    auto token = jwt::create()
        .set_issuer("SubManager")
        .set_type("JWT")
        .set_subject(std::to_string(user_id))
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24 * 365))
        .sign(jwt::algorithm::hs256{DataManagementService::secret_key});
    
    return token;
}

int DataManagementService::verifyJwtToken(const std::string& token){
    try{
        auto decoded = jwt::decode(token);
        jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{DataManagementService::secret_key})
        .with_issuer("SubManager")
        .verify(decoded);

        cout << "subject id:" << decoded.get_subject() << "\n";
        return std::stoi(decoded.get_subject());
    } catch (const std::exception &e){
        return -1;
    }
}

int DataManagementService::isUserAuthenticated(const crow::request &req,
    crow::response &res, sql::Connection *conn) {
    // Try extract username and password from the request.
    try {
        std::string username = req.url_params.get("username");
        std::string password = req.url_params.get("password");

        // Try query the databse
        try {
            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.authenticationQuery(username,
            password);
            sql::ResultSet *result = stmt->executeQuery(query);

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
            res.code = 401;  // Unauthorized
            res.write("Authentication failed \n");
            res.end();
        }
        catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            res.write("Database Error: " + std::string(e.what()) + "\n");
            res.end();
        }
    }
    catch (const std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        res.write("Invalid request \n");
        res.end();
        return -1;
    }

    return -1;
}

void DataManagementService::getCompanyInfo(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();

    if (companyId != -1) {
        sql::Statement *stmt = conn->createStatement();
        std::string query = queryGenerator.companyInfoQuery(companyId);

        try {
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            if (queryResult->rowsCount() > 0) {
                std::string companyData = "";
                while (queryResult->next()) {
                    companyData += "Company ID: " +
                    queryResult->getString("company_id") + "; ";
                    companyData += "Company Name: " +
                    queryResult->getString("company_name") + "; ";
                    companyData += "Company email: " +
                    queryResult->getString("email") + " \n";
                }
                res.code = 200;  // OK
                res.write("Result: " + companyData);
                res.end();
            } else {
                res.code = 200;  // OK
                res.write("No Query Found");
                res.end();
            }

            delete queryResult;
            delete stmt;
        }
        catch (const sql::SQLException &e) {
            res.code = 500;
            res.write("Database Error: " + std::string(e.what()) + "\n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::addCompany(const crow::request &req,
    crow::response &res) {
    sql::Connection *conn = DBConnect();
    // Try extract companyId, email, hashPwd, and companyName from the request.
    try {
        auto bodyInfo = crow::json::load(req.body);
        std::string email = bodyInfo["email"].s();
        std::string companyName = bodyInfo["company_name"].s();;

        try {
            std::string query = queryGenerator.addCompanyInfoQuery(email, companyName);
            sql::Statement *stmt = conn->createStatement();
            stmt->execute(query);

            sql::Statement *stmt2 = conn->createStatement();
            sql::ResultSet *queryResult = stmt2->executeQuery("SELECT LAST_INSERT_ID() as last_id");

            if(queryResult->next()){
                int company_id = queryResult->getInt("last_id");
                std::string jwtToken = generateJwtToken(company_id);
                
                res.code = 200;  // OK
                res.write("Add Company Success \n");
                res.write("Please save your JWT token: " + jwtToken + "\n");
                res.end();

            }else{
                res.code = 500;  // Internal Server Error
                res.write("Add Company Failed due to database issue.\n");
                res.end();
            }

            delete stmt;
            delete stmt2;
            delete queryResult;
        }
        catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            int errorCode = e.getErrorCode();
            if(errorCode == 1062){ // duplicate company email
                res.write("Add Company Error: You have already registered with this email,"
                " if you lost your JWT token, please apply for a new one.");
            }
            else{
                 res.write("Add Company Error: " + std::string(e.what()) + "\n");
            }
           
            res.end();
        }
        res.end();
    }
    catch (const std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        res.write("Invalid request \n");
        res.end();
    }
    DBDisConnect(conn);
}

void DataManagementService::recoverCompany(const crow::request& req, crow::response& res){
    sql::Connection *conn = DBConnect();

    try{
        auto bodyInfo = crow::json::load(req.body);
        std::string email = bodyInfo["email"].s();

        try {
           
            sql::Statement *stmt = conn->createStatement();
            sql::ResultSet *queryResult = stmt->executeQuery("SELECT * FROM service.company_table WHERE email = '"
            + email + "'");

            if(queryResult->next()){
                int company_id = queryResult->getInt("company_id");
                std::string company_name =  queryResult->getString("company_name");
                std::string jwtToken = generateJwtToken(company_id);
                
                CURL *curl = curl_easy_init();
                
                if(curl){
                    const char *url = "https://api.sendgrid.com/v3/mail/send";
                    struct curl_slist* headers = NULL;
                    headers = curl_slist_append(headers, "Authorization: Bearer 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");
                    headers = curl_slist_append(headers, "Content-Type: application/json");

                    // Set the JSON payload with email details
                    std::string jsonPayload = "{\"personalizations\":[{\"to\": [{\"email\":\"" + email + "\"}]}],\"from\": {\"email\":\"hl3608@columbia.edu\"},\"subject\": \"SubManager New Token\",\"content\": [{\"type\": \"text/plain\", \"value\": \"Your new token is: " + jwtToken + " \"}]}";


                    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //verbose for debug
                    curl_easy_setopt(curl, CURLOPT_URL, url);
                    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

                    try{

                        CURLcode curl_res = curl_easy_perform(curl);
                        

                        // Check the response
                        if (curl_res != CURLE_OK) {
                            res.code = 500;  // Internal Server Error
                            std::string errMsg(curl_easy_strerror(curl_res));
                            res.write("Failed to send email" +  errMsg + "\n");
                            res.end();
                            
                            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_res));
                        } else{
                            res.code = 200;  // OK
                            res.write("An email has been send to your address \n");
                            res.end();
                        }
                        
                        // Clean up
                        curl_slist_free_all(headers);
                        curl_easy_cleanup(curl);
                    } catch (const std::exception &e) {
                        res.code = 500;  // Internal Server Error
                        res.write("Failed to send email" + std::string(e.what()) + "\n");
                        res.end();
                    }
                } else{
                    res.code = 500;  // Internal Server Error
                    res.write("Failed to send email with curl error\n");
                    res.end();
                }
            } else{
                res.code = 400;  // Bad Request
                res.write("Your email has not registed\n");
                res.end();
            }

            delete stmt;
            delete queryResult;
        } catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            int errorCode = e.getErrorCode();
            if(errorCode == 1062){ // duplicate company email
                res.write("Add Company Error: You have already registered with this email,"
                " if you lost your JWT token, please apply for a new one.");
            }
            else{
                 res.write("Add Company Error: " + std::string(e.what()) + "\n");
            }
           
            res.end();
        }
    } catch (const std::exception &e){
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        res.write("Invalid request \n");
        res.end();
    }

    DBDisConnect(conn);
}

void DataManagementService::addMember(const crow::request &req,
    crow::response &res) {
    sql::Connection *conn = DBConnect();
    
    try {
        auto bodyInfo = crow::json::load(req.body);
        std::string memberId = bodyInfo["member_id"].s();
        std::string firstName = bodyInfo["first_name"].s();
        std::string lastName = bodyInfo["last_name"].s();
        std::string email = bodyInfo["email"].s();
        std::string phoneNumber = bodyInfo["phone_number"].s();
        std::string memberStatus = bodyInfo["member_status"].s();

        try {
            std::string query = queryGenerator.addMemberQuery(memberId,
                firstName, lastName, email, phoneNumber, memberStatus);
            sql::Statement *stmt = conn->createStatement();
            stmt->execute(query);
            res.code = 200;  // OK
            res.write("Add Member Success \n");
            res.end();
        }
        catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            res.write("Add Member Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        res.end();
    }
    catch (std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        res.write("Invalid request \n");
        res.end();
    }
    DBDisConnect(conn);
}

void DataManagementService::addSubscription(const crow::request &req,
    crow::response &res) {
    sql::Connection *conn = DBConnect();
    
    try {
        auto bodyInfo = crow::json::load(req.body);
        std::string subscriptionId = bodyInfo["subscription_id"].s();
        std::string memberId = bodyInfo["member_id"].s();
        std::string companyId = bodyInfo["company_id"].s();
        std::string subscriptionType = bodyInfo["subscription_type"].s();
        std::string subscriptionStatus = bodyInfo["subscription_status"].s();
        std::string nextDueDate = bodyInfo["next_due_date"].s();
        std::string startDate = bodyInfo["start_date"].s();
        std::string billingInfo = bodyInfo["billing_info"].s();

        try {
            std::string queryString =
                "Insert into service.subscription_table Values (" +
                subscriptionId + ", " + memberId + ", " + companyId + ", '" +
                subscriptionType + "', '" + subscriptionStatus + "', '" +
                nextDueDate + "', '" + startDate + "', '" + billingInfo + "');";
            sql::Statement *stmt = conn->createStatement();
            stmt->execute(queryString);
            res.code = 200;  // OK
            res.write("Add Subscription Success \n");
            res.end();
        }
        catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            res.write("Add Subscription Error: " +
                std::string(e.what()) + "\n");
            res.end();
        }
    }
    catch (std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        res.write("Invalid request \n");
        res.end();
    }

    DBDisConnect(conn);
}

sql::Connection *DBConnect() {
    // Database connection
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;

    driver = sql::mysql::get_mysql_driver_instance();
    // Connect IP adress, username, password
    conn = driver->connect("tcp://172.22.32.1:3306", "admin", "debugteam");

    return conn;
}

void DBDisConnect(sql::Connection *conn) {
    delete conn;
}
