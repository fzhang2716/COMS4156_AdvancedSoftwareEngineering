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

std::string DataManagementService::generateJwtToken(int client_id){
    // create a token valid for 1 year
    auto token = jwt::create()
        .set_issuer("SubManager")
        .set_type("JWT")
        .set_subject(std::to_string(client_id))
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


void DataManagementService::changeCompany(const crow::request &req,
    crow::response &res) {
    sql::Connection *conn = DBConnect();
    // Try extract companyId, email, hashPwd, and companyName from the request.
    try {
        auto bodyInfo = crow::json::load(req.body);
        int companyId = bodyInfo["company_id"].i();
        std::string email = bodyInfo["email"].s();
        std::string companyName = bodyInfo["company_name"].s();
        sql::Statement *stmt = conn->createStatement();
        std::string query = queryGenerator.companyInfoQuery(companyId);
        sql::ResultSet *queryResult = stmt->executeQuery(query);
        if (queryResult->rowsCount() > 0) {
            std::string query = queryGenerator.updateCompanyInfoQuery(email, companyName, companyId);
            stmt->executeQuery(query);
        } else {
            res.code = 200;  // OK
            res.write("No Query Found To Update");
            res.end();
        }
    }
    catch(const sql::SQLException &e) {
        if (e.getErrorCode() == 0) {
                res.code = 200;  // OK
                res.write("Update success");
        } else {
            res.code = 500;
            res.write("Add Company Error: " + std::string(e.what()) + "\n");
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
                    std::string auth = "Authorization: Bearer "+ DataManagementService::sendGrid_key;
                    headers = curl_slist_append(headers, auth.c_str());
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
                res.write("Your email has not been registered\n");
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
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    
    if(companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string firstName = bodyInfo["first_name"].s();
            std::string lastName = bodyInfo["last_name"].s();
            std::string email = bodyInfo["email"].s();
            std::string phoneNumber = bodyInfo["phone_number"].s();

            try {
                std::string query = queryGenerator.addMemberQuery(std::to_string(companyId),
                    firstName, lastName, email, phoneNumber);
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
    }
    DBDisConnect(conn);
}

void DataManagementService::removeMember(const crow::request &req,
    crow::response &res, int companyId, std::string removeEmail) {
    sql::Connection *conn = DBConnect();

    if (companyId != -1) {
        sql::Statement *searchStmt = conn->createStatement();
        std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, removeEmail);
        try {
            sql::ResultSet *queryResult = searchStmt->executeQuery(query);
            if (queryResult->rowsCount() > 0) {
                sql::Statement *DeleteStmt = conn->createStatement();
                std::string query = queryGenerator.deleteMemeberByCompanyIdAndEmailQuery(companyId, removeEmail);
                DeleteStmt->executeQuery(query);
                res.code = 204;
                res.write("Delete success");
            } else {
                res.code = 404;
                res.write("No matching Memeber found");
            }
            res.end();
        }
        catch (sql::SQLException &e) {
            // Catch any SQL errors
            if (e.getErrorCode() == 0) {
                res.code = 204;  // 204 is standard code for success delete
                res.write("Delete Memeber success");
            } else {
                res.code = 500;
                res.write("Delete Memeber Error: " + std::string(e.what()) + "\n");
            }
            res.end();
        }
        catch (std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::changeMemberInfo(const crow::request &req,
    crow::response &res) {
    sql::Connection *conn = DBConnect();
    try {
        auto bodyInfo = crow::json::load(req.body);
        int companyId = bodyInfo["company_id"].i();
        std::string email = bodyInfo["email"].s();
        // email and company id is the primary key, might not needed if there is a different way to identify memeber
        std::string firstName = bodyInfo["first_name"].s();
        std::string lastName = bodyInfo["last_name"].s();
        std::string phoneNumber = bodyInfo["phone_number"].s();

        sql::Statement *stmt = conn->createStatement();
        std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, email);
        sql::ResultSet *queryResult = stmt->executeQuery(query);
        if (queryResult->rowsCount() > 0) {
            std::string query = queryGenerator.updateMemberInfoQuery(std::to_string(companyId),
            firstName, lastName, email, phoneNumber);
            stmt->executeQuery(query);
        } else {
            res.code = 404;  // OK
            res.write("No Query Found To Update");
            res.end();
        }
    }
    catch(const sql::SQLException &e) {
        if (e.getErrorCode() == 0) {
                res.code = 200;  // OK
                res.write("Update success");
        } else {
            res.code = 500;
            res.write("Change Member Info Error: " + std::string(e.what()) + "\n");
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

void DataManagementService::addSubscription(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string memberEmail = bodyInfo["member_email"].s();
            std::string subscriptionType = bodyInfo["subscription_type"].s();
            std::string subscriptionStatus = bodyInfo["subscription_status"].s();
            std::string nextDueDate = bodyInfo["next_due_date"].s();
            std::string startDate = bodyInfo["start_date"].s();
            std::string billingInfo = bodyInfo["billing_info"].s();


            try {
                std::string queryString =
                    "Insert into service.subscription_table (member_email, company_id, subscription_type, subscription_status, next_due_date, start_date, billing_info) Values ('" + memberEmail + "', '" + std::to_string(companyId) + "', '" +
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
    }

    DBDisConnect(conn);
}

sql::Connection *DBConnect() {
    // Database connection
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;

    driver = sql::mysql::get_mysql_driver_instance();
    // Connect IP adress, username, password (TODO: replace IP adress)
    conn = driver->connect("tcp://database-1.cwyxftpcqhs4.us-east-2.rds.amazonaws.com", "admin", "debugteam");

    return conn;
}

void DBDisConnect(sql::Connection *conn) {
    delete conn;
}
