/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include "data_management.hpp"
#include "utils.hpp"
#include <jwt-cpp/jwt.h>
#include <exception>
#include <cstdlib>
#include <curl/curl.h>
#include <json/json.h>
#include <vector>
#include "analyze_data.hpp"
#include "send_attachment.cpp"

using namespace std;
Query queryGenerator;

std::string DataManagementService::generateSessionSecret(){
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int length = 32;
    std::string sessionSecret;
    for (int i = 0; i < length; ++i) {
        sessionSecret += characters[std::rand() % characters.length()];
    }
    return sessionSecret;
}

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
            Json::Value jsonResponse;
            if (queryResult->next()) {
                jsonResponse["company_id"] = static_cast<std::string>(queryResult->getString("company_id"));
                jsonResponse["company_name"] = static_cast<std::string>(queryResult->getString("company_name"));
                jsonResponse["email"] = static_cast<std::string>(queryResult->getString("email"));
                res.code = 200;  // OK
            } else {
                jsonResponse["error"] = "Error getting your company's information";
                res.code = 204; // No Content
            }
            res.add_header("Content-Type", "application/json");
            res.write(jsonResponse.toStyledString());
            res.end();
            
            delete queryResult;
            delete stmt;
        }
        catch (const sql::SQLException &e) {
            res.code = 500;
            res.write("Database Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::addCompany(const crow::request &req,
    crow::response &res) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;
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
                jsonResponse["msg"] = "Add Company Success";
                jsonResponse["tokenMsg"] = jwtToken;
                res.write(jsonResponse.toStyledString());
                res.end();

            }else{
                res.code = 500;  // Internal Server Error
                jsonResponse["error"] = "Add Company Failed due to database issue.";
                res.write(jsonResponse.toStyledString());
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
                jsonResponse["error"] = "Add Company Error: You have already registered with this email,"
                " if you lost your JWT token, please apply for a new one.";
                res.write(jsonResponse.toStyledString());
            }
            else{
                jsonResponse["error"] = "Add Company Error: " + std::string(e.what());
                res.write(jsonResponse.toStyledString());

            }
           
            res.end();
        }
        res.end();
    }
    catch (const std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        jsonResponse["error"] = "Invalid request";
        res.write(jsonResponse.toStyledString());
        res.end();
    }
    DBDisConnect(conn);
}


void DataManagementService::changeCompany(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    
    if (companyId != -1){
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string companyName = bodyInfo["company_name"].s(); 
            std::string query = queryGenerator.updateCompanyInfoQuery(companyName, companyId);
            sql::Statement *stmt = conn->createStatement();

            stmt->execute(query);
            res.code = 200;
            res.write("Update Company Success \n");
            res.end();
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("Add Company Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getCompanyMembers(const crow::request& req, crow::response& res, int companyId){
    sql::Connection *conn = DBConnect();
    auto page_param = req.url_params.get("page");
    auto pageSize_param = req.url_params.get("pageSize");
    
    // Pagination dafualt setting
    int page = 1;
    int pageSize = 10;

    if(page_param != nullptr){
        page = std::stoi(page_param);
    }
    if(pageSize_param != nullptr){
        pageSize = std::stoi(pageSize_param);
    }
        
    try{
        // Get total number of members
        sql::Statement *countStmt = conn->createStatement();
        sql::ResultSet *countResult = countStmt->executeQuery("SELECT COUNT(*) as count FROM service.member_table WHERE company_id = '"
            + std::to_string(companyId) + "'");
        
        countResult->next();
        int totalMembers = countResult->getInt("count");
        int totalPages = (totalMembers + pageSize - 1)/pageSize; //cround up to integer

        sql::Statement *stmt = conn->createStatement();
        sql::ResultSet *queryResult = stmt->executeQuery("SELECT * FROM service.member_table WHERE company_id = '"
            + std::to_string(companyId) + "'" + " LIMIT " + std::to_string(pageSize) + " OFFSET " + std::to_string((page-1)*pageSize));
        
        Json::Value jsonResponse;
        jsonResponse["total_members"] = std::to_string(totalMembers);
        jsonResponse["total_pages"] = std::to_string(totalPages);
        Json::Value membersArray(Json::arrayValue);
        while (queryResult->next()) {
            Json::Value memberJson;
            memberJson["email"] = static_cast<std::string>(queryResult->getString("email"));
            memberJson["first_name"] = static_cast<std::string>(queryResult->getString("first_name"));
            memberJson["last_name"] = static_cast<std::string>(queryResult->getString("last_name"));
            memberJson["phone_number"] = static_cast<std::string>(queryResult->getString("phone_number"));
            
            membersArray.append(memberJson);
        }
        jsonResponse["members"] = membersArray;
        
        res.code = 200;
        res.add_header("Content-Type", "application/json");    
        res.write(jsonResponse.toStyledString());
        res.end();
        delete queryResult;
        delete stmt;

    } catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("Get Company Members Error: " + std::string(e.what()) + "\n");
            res.end();
    } catch (const std::exception &e) {
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
    Json::Value jsonResponse;
    if(companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string firstName = bodyInfo["first_name"].s();
            std::string lastName = bodyInfo["last_name"].s();
            std::string email = bodyInfo["email"].s();
            std::string password = bodyInfo["password"].s();
            std::string phoneNumber = bodyInfo["phone_number"].s();

            try {   
                std::string query = queryGenerator.addMemberQuery(companyId,
                    firstName, lastName, email, password, phoneNumber);
                sql::Statement *stmt = conn->createStatement();
                stmt->execute(query);
                res.code = 200;  // OK
                res.add_header("Content-Type", "application/json");
                jsonResponse["msg"] = "Add Member Success";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
            catch (sql::SQLException &e) {
                // Catch any SQL errors
                res.code = 500;  // Internal Server Error
                jsonResponse["msg"] = "Add Member Error: " + std::string(e.what());
                res.write(jsonResponse.toStyledString());
                res.end();
            }
            res.end();
        }
        catch (std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["msg"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}


std::string DataManagementService::memberLogin(const crow::request& req, crow::response& res, int companyId){
    sql::Connection *conn = DBConnect();
    std::string sessionEmail = "";

    if(companyId != -1){
        try{
            auto bodyInfo = crow::json::load(req.body);
            std::string email = bodyInfo["email"].s();
            std::string password = bodyInfo["password"].s();
            
            try {
                
                sql::Statement *stmt = conn->createStatement();
                std::string query = queryGenerator.searchMemeberForLogin(companyId, email, password);
                sql::ResultSet *queryResult = stmt->executeQuery(query);
                if(queryResult->next()){
                    sessionEmail = email;
                    res.code = 200;
                }else{
                    res.code = 401; // Unauthorized
                }
                delete queryResult;
                delete stmt;
            }
            catch (sql::SQLException &e) {
                // Catch any SQL errors
                res.code = 500;  // Internal Server Error
                res.write("Login Error: " + std::string(e.what()) + "\n");
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
    return sessionEmail;
}



void DataManagementService::removeMember(const crow::request &req,
    crow::response &res, int companyId, std::string removeEmail) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
        sql::Statement *searchStmt = conn->createStatement();
        std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, removeEmail);
        try {
            sql::ResultSet *queryResult = searchStmt->executeQuery(query);
            if (queryResult->rowsCount() > 0) {
                sql::Statement *DeleteStmt = conn->createStatement();
                std::string query = queryGenerator.deleteMemeberByCompanyIdAndEmailQuery(companyId, removeEmail);
                DeleteStmt->executeUpdate(query);
                res.code = 204;
                jsonResponse["msg"] = "Delete Member Success";
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
                res.code = 400;
                jsonResponse["error"] = "No Matching Memeber Found";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
        }
        catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            jsonResponse["error"] = "SQL Exception - Error code: " + std::to_string(e.getErrorCode()) + "\n" + "SQL State: " + e.getSQLState() + "\n" +"What: " + std::string(e.what()) + "\n";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request \n";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::changeMemberInfoAdmin(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1){
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string email = bodyInfo["email"].s();
            // email and company id is the primary key, might not needed if there is a different way to identify memeber
            std::string firstName = bodyInfo["first_name"].s();
            std::string lastName = bodyInfo["last_name"].s();
            std::string phoneNumber = bodyInfo["phone_number"].s();

            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, email);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            if (queryResult->rowsCount() > 0) {
                std::string query = queryGenerator.updateMemberInfoQuery(companyId,
                firstName, lastName, email, phoneNumber);
                stmt->execute(query);
                res.code = 200;  // OK
                jsonResponse["msg"] = "Update success";
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
                res.code = 400;   // Bad Request
                res.write("No Query Found To Update");
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["err"] = "Change Member Info Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["err"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::changeMemberInfo(const crow::request& req, crow::response& res, int companyId, std::string email){
    sql::Connection *conn = DBConnect();
    if (companyId != -1){
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string firstName = bodyInfo["first_name"].s();
            std::string lastName = bodyInfo["last_name"].s();
            std::string phoneNumber = bodyInfo["phone_number"].s();

            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, email);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            if (queryResult->rowsCount() > 0) {
                std::string query = queryGenerator.updateMemberInfoQuery(companyId,
                firstName, lastName, email, phoneNumber);
                stmt->execute(query);
                res.code = 200;  // OK
                res.write("Update success");
                res.end();
            } else {
                res.code = 400;   // Bad Request
                res.write("No Query Found To Update");
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("Change Member Info Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getMemberInfo(const crow::request& req, crow::response& res, int companyId, std::string email){
    sql::Connection *conn = DBConnect();
    if (companyId != -1){
        try {
            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, email);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            
            Json::Value jsonResponse;
            // Tthere is at least one row in the result set
            if (queryResult->next()) {
                jsonResponse["email"] = static_cast<std::string>(queryResult->getString("email"));
                jsonResponse["first_name"] = static_cast<std::string>(queryResult->getString("first_name"));
                jsonResponse["last_name"] = static_cast<std::string>(queryResult->getString("last_name"));
                jsonResponse["phone_number"] = static_cast<std::string>(queryResult->getString("phone_number"));
                res.code = 200;
            } else {
                jsonResponse["error"] = "No member found for the specified company and email";
                res.code = 400; 
            }   

            res.add_header("Content-Type", "application/json");
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("Change Member Info Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}



void DataManagementService::addSubscription(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string memberEmail = bodyInfo["member_email"].s();
            std::string subscriptionName = bodyInfo["subscription_name"].s();
            std::string subscriptionType = bodyInfo["subscription_type"].s();
            std::string subscriptionStatus = bodyInfo["subscription_status"].s();
            std::string nextDueDate = bodyInfo["next_due_date"].s();
            std::string startDate = bodyInfo["start_date"].s();
            std::string billingInfo = bodyInfo["billing_info"].s();

            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, memberEmail);
            sql::ResultSet *queryResult = stmt->executeQuery(query);

            if (queryResult->rowsCount() > 0) {
                try {
                    std::string queryString = queryGenerator.addSubscriptionQuery(memberEmail, companyId, subscriptionName, subscriptionType, subscriptionStatus, nextDueDate, startDate, billingInfo);
                    sql::Statement *stmt = conn->createStatement();
                    stmt->execute(queryString);
                    res.code = 200;  // OK
                    jsonResponse["msg"] = "Add Subscription Success";
                    res.write(jsonResponse.toStyledString());
                    res.end();
                }
                catch (sql::SQLException &e) {
                    // Catch any SQL errors
                    res.code = 500;  // Internal Server Error
                    jsonResponse["error"] = "Add Subscription Error: " +
                        std::string(e.what());
                    res.end();
                }
            }
            else{
                res.code = 400;  // Bad Request
                jsonResponse["error"] = "Member Not Exists";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Add Subscription Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }

    DBDisConnect(conn);
}


void DataManagementService::updateSubscription(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if(companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string email = bodyInfo["email"].s();
            std::string subscriptionName = bodyInfo["subscription_name"].s();
            std::string newAction = bodyInfo["new_action"].s();

            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchSubscriptioByCompanyIdAndEmailAndSubscriptionNameQuery
            (companyId, email, subscriptionName);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            if (queryResult->rowsCount() > 0) {
                std::string subscriptionStatus = "";

                while (queryResult->next()) {
                    subscriptionStatus += queryResult->getString("subscription_status");
                }
                std::string currentTime = "" + getCurrentDateTime();
                std::string query = queryGenerator.
                updateSubscriptionAction(companyId, email, subscriptionName, subscriptionStatus, currentTime, newAction);
                stmt->execute(query);
                res.code = 200;
                jsonResponse["msg"] = "Update Success";
                res.write(jsonResponse.toStyledString());

            } else {
                res.code = 400;
                jsonResponse["err"] = "No Query Found To Update";
                res.write(jsonResponse.toStyledString());

            }
            res.end();
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["err"] = "Change Member Info Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request

            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::updateSubscriptionAdmin(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();

    if(companyId != -1) {
        try {
            std::cout << "function called";
            auto bodyInfo = crow::json::load(req.body);
            std::string subscriptionId = bodyInfo["subscription_id"].s(); //Required
            
            std::string subscriptionName = bodyInfo["subscription_name"].s();
            std::string subscriptionType = bodyInfo["subscription_type"].s();
            std::string subscriptionStatus = bodyInfo["subscription_status"].s();
            std::string startDate = bodyInfo["start_date"].s();
            std::string nextDueDate = bodyInfo["next_due_date"].s();
            std::string billingInfo = bodyInfo["billing_info"].s();
            
            sql::Statement *searchStmt = conn->createStatement();
            std::string searchQuery = "SELECT * from service.subscription_table WHERE company_id = " + std::to_string(companyId) + " AND subscription_id = '" + subscriptionId + "';";
            sql::ResultSet *queryResult = searchStmt->executeQuery(searchQuery);
            if (queryResult->rowsCount() > 0) {

                std::string query = queryGenerator.
                updateSubscriptionAdmin(subscriptionId, subscriptionName, subscriptionType, subscriptionStatus, startDate, nextDueDate, billingInfo);
                sql::Statement *stmt = conn->createStatement();
                stmt->execute(query);
                res.code = 200;
                res.write("Update Success");
                res.end();
            }
            else {
                res.code = 400;
                res.write("No subscription found or you don't have permission to modify this subscription.");
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("Update Subscription Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.write("What: " + std::string(e.what()) + "\n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::viewSubscriptions(const crow::request& req, 
crow::response& res, int companyId, bool isAdmin){
    sql::Connection *conn = DBConnect();

    if(companyId != -1) {
        try{
            auto bodyInfo = crow::json::load(req.body);
            std::string email = bodyInfo["email"].s();
            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchSubscriptioByCompanyIdAndEmailQuery(companyId, email);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            int total_subscriptions = static_cast<int>(queryResult->rowsCount());
            
            Json::Value jsonResponse;
            Json::Value subscriptionsArray(Json::arrayValue);
            // Tthere is at least one row in the result set
            while (queryResult->next()) {
                Json::Value subscriptionJson;
                subscriptionJson["subscription_id"] = std::to_string(queryResult->getInt("subscription_id"));
                subscriptionJson["subscription_name"] = static_cast<std::string>(queryResult->getString("subscription_name"));
                subscriptionJson["subscription_type"] = static_cast<std::string>(queryResult->getString("subscription_type"));
                subscriptionJson["subscription_status"] = static_cast<std::string>(queryResult->getString("subscription_status"));
                subscriptionJson["start_date"] = static_cast<std::string>(queryResult->getString("start_date"));
                subscriptionJson["next_due_date"] = static_cast<std::string>(queryResult->getString("next_due_date"));
                subscriptionJson["billing_info"] = static_cast<std::string>(queryResult->getString("billing_info"));
                
                if(isAdmin){
                    subscriptionJson["last_action"] = static_cast<std::string>(queryResult->getString("last_action"));
                    subscriptionJson["last_action_date"] = static_cast<std::string>(queryResult->getString("last_action_date"));
                }
                subscriptionsArray.append(subscriptionJson);
                
            }
            jsonResponse["total_subscriptions"] = std::to_string(total_subscriptions);
            jsonResponse["subscriptions"] = subscriptionsArray;

            res.code = 200;
            res.add_header("Content-Type", "application/json");    
            res.write(jsonResponse.toStyledString());
            res.end();
            delete queryResult;
            delete stmt;
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("View member's subscription Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getExpiringSubscriptionByTime(const crow::request &req,
    crow::response &res, int companyId) {
    Json::Value jsonObject;
    sql::Connection *conn = DBConnect();

    if(companyId != -1) {
        try {
            int rangeDays;
            std::string subscriptionName;
            int counter = 0;
            
            rangeDays = crow::utility::lexical_cast<int>(req.url_params.get("days"));
            std::string targetTime = "" + timeAddition(rangeDays);

            if (req.url_params.get("subscription_name") == nullptr) {
                subscriptionName = "";
            } else {
                subscriptionName = crow::utility::lexical_cast<string>(req.url_params.get("subscription_name"));
            }

            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchFutureExpireSubscriptioByCompanyIdAndEmailAndrangeDaysAndsubscriptionId(companyId, targetTime, subscriptionName);
            sql::ResultSet *queryResult = stmt->executeQuery(query);

            if (queryResult->rowsCount() > 0) {
                std::string companyData = "";
                while (queryResult->next()) {
                    std::string currEmail = queryResult->getString("member_email");
                    jsonObject[std::to_string(counter)] = currEmail;
                    counter += 1;
                }                
            }
            jsonObject["number"] = std::to_string(counter);
            jsonObject["target_time"] = targetTime;

            std::string jsonString = jsonObject.toStyledString();
            res.code = 200;  // OK
            res.write(jsonString);
            res.end();
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            res.write("Change Member Info Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            res.write("Invalid request \n");
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::sendReminder(const crow::request &req,
                                         crow::response &res, int companyId) {
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            int num = bodyInfo["number"].i();
            int count = 0;
            if (num == 0) {
                res.code = 400;
                jsonResponse["error"] = "No email to send";
                res.write(jsonResponse.toStyledString());
            } else {
                std::string targetTime = bodyInfo["target_time"].s();
                std::cout << targetTime;
                while (count < num) {
                    std::string email = bodyInfo[std::to_string(count)].s();

                    std::cout << email;
                    CURL *curl = curl_easy_init();
                    if(curl) {
                        const char *url = "https://api.sendgrid.com/v3/mail/send";
                        struct curl_slist* headers = NULL;
                        std::string auth = "Authorization: Bearer "+ DataManagementService::sendGrid_key;
                        headers = curl_slist_append(headers, auth.c_str());
                        headers = curl_slist_append(headers, "Content-Type: application/json");

                        // Set the JSON payload with email details
                        std::string jsonPayload = "{\"personalizations\":[{\"to\": [{\"email\":\"" + email +
                        "\"}]}],\"from\": {\"email\":\"hl3608@columbia.edu\"},\"subject\": \"SubManager New Token\",\"content\": [{\"type\": \"text/plain\", \"value\": \"Your subscription is about to expire before "
                        + targetTime + " \"}]}";

                        curl_easy_setopt(curl, CURLOPT_URL, url);
                        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

                        try {
                            CURLcode curl_res = curl_easy_perform(curl);

                            // Check the response
                            if (curl_res != CURLE_OK) {
                                res.code = 500;  // Internal Server Error
                                std::string errMsg(curl_easy_strerror(curl_res));
                                res.write("Failed to send email" +  errMsg + "\n");
                                res.end();
                                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_res));
                            }
                            // Clean up
                            curl_slist_free_all(headers);
                            curl_easy_cleanup(curl);
                        } catch (const std::exception &e) {
                            res.code = 500;  // Internal Server Error
                            jsonResponse["error"] = "Failed to send email" + std::string(e.what());
                            res.write(jsonResponse.toStyledString());
                            res.end();
                        }
                    } else {
                        res.code = 500;  // Internal Server Error
                        jsonResponse["error"] = "Failed to send email with curl error";
                        res.write(jsonResponse.toStyledString());
                        res.end();
                    }
                    count += 1;
                }
                res.code = 200;
                jsonResponse["msg"] = "Send successfully";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
        } catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
}
void DataManagementService::deleteByString(const crow::request &req,
                                         crow::response &res, int companyId) {
    auto bodyInfo = crow::json::load(req.body);
    std::string sqlCommed = bodyInfo["sqlCommed"].s();
    sql::Connection *conn = DBConnect();
    sql::Statement *stmt = conn->createStatement();
    stmt->executeQuery(sqlCommed);
    DBDisConnect(conn);
    res.end();
}

void DataManagementService::analyzeSubDuration(const crow::request& req, crow::response& res, int companyId) {
    sql::Connection *conn = DBConnect();

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string email = bodyInfo["email"].s();
            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchSubscriptionDurationByCompanyId(companyId);
            sql::ResultSet *queryResult = stmt->executeQuery(query);

            std::vector<float> durations;

            while (queryResult->next()) {
                float duration = static_cast<float>(queryResult->getInt("duration"));
                durations.push_back(duration);
            }

            Analyze Analysis;
            std::string analysis = Analysis.analyze(durations, "Durations (day)");
            send(email, "", analysis);

            res.code = 200;
            res.add_header("Content-Type", "application/json");
            res.write("Analyze successfully");
            res.end();
            delete queryResult;
            delete stmt;
        }
        catch (const sql::SQLException &e) {
            res.code = 500;
            res.write("Analyze Subscription Duration Error: " + std::string(e.what()) + "\n");
            res.end();
        }
        catch (const std::exception &e) {
            res.code = 400;
            res.write("Invalid Request \n");
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
