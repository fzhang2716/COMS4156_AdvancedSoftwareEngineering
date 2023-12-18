/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include "./data_management.hpp"
#include <jwt-cpp/jwt.h>
#include <curl/curl.h>
#include <json/json.h>
#include <exception>
#include <cstdlib>
#include <vector>
#include "./utils.hpp"
#include "analyze_data.cpp"
#include "send_attachment.cpp"
#include "make_attachment.cpp"

using namespace std;  // NOLINT
Query queryGenerator;

std::string DataManagementService::generateSessionSecret() {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int length = 32;
    std::string sessionSecret;
    for (int i = 0; i < length; ++i) {
        sessionSecret += characters[std::rand() % characters.length()];
    }
    return sessionSecret;
}

std::string DataManagementService::generateJwtToken(int client_id) {
    // create a token valid for 1 year
    auto token = jwt::create()
        .set_issuer("SubManager")
        .set_type("JWT")
        .set_subject(std::to_string(client_id))
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24 * 365))
        .sign(jwt::algorithm::hs256{DataManagementService::secret_key});

    return token;
}

int DataManagementService::verifyJwtToken(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{DataManagementService::secret_key})
        .with_issuer("SubManager")
        .verify(decoded);

        cout << "subject id:" << decoded.get_subject() << "\n";
        return std::stoi(decoded.get_subject());
    } catch (const std::exception &e) {
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
                res.code = 204;  // No Content
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

            if (queryResult->next()) {
                int company_id = queryResult->getInt("last_id");
                std::string jwtToken = generateJwtToken(company_id);
                res.code = 200;  // OK
                jsonResponse["msg"] = "Add Company Success";
                jsonResponse["tokenMsg"] = jwtToken;
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
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
            if (errorCode == 1062) {  // duplicate company email
                jsonResponse["error"] = "Add Company Error: You have already registered with this email,"
                " if you lost your JWT token, please apply for a new one.";
                res.write(jsonResponse.toStyledString());
            } else {
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
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string companyName = bodyInfo["company_name"].s();
            std::string query = queryGenerator.updateCompanyInfoQuery(companyName, companyId);
            sql::Statement *stmt = conn->createStatement();

            stmt->execute(query);
            res.code = 200;
            jsonResponse["msg"] ="Update Company Success";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] ="Add Company Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] ="Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getCompanyMembers(const crow::request& req, crow::response& res, int companyId) {
    sql::Connection *conn = DBConnect();
    auto page_param = req.url_params.get("page");
    auto pageSize_param = req.url_params.get("pageSize");
    Json::Value jsonResponse;
    // Pagination dafualt setting
    int page = 1;
    int pageSize = 10;

    if (page_param != nullptr) {
        page = std::stoi(page_param);
    }
    if (pageSize_param != nullptr) {
        pageSize = std::stoi(pageSize_param);
    }
    try {
        // Get total number of members
        sql::Statement *countStmt = conn->createStatement();
        sql::ResultSet *countResult = countStmt->executeQuery("SELECT COUNT(*) as count FROM service.member_table WHERE company_id = '"
            + std::to_string(companyId) + "'");
        countResult->next();
        int totalMembers = countResult->getInt("count");
        int totalPages = (totalMembers + pageSize - 1)/pageSize;  // cround up to integer

        sql::Statement *stmt = conn->createStatement();
        sql::ResultSet *queryResult = stmt->executeQuery("SELECT * FROM service.member_table WHERE company_id = '"
            + std::to_string(companyId) + "'" + " LIMIT " + std::to_string(pageSize) + " OFFSET " + std::to_string((page-1)*pageSize));

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
            jsonResponse["error"] ="Get Company Members Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
    } catch (const std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        jsonResponse["error"] ="Invalid request";
        res.write(jsonResponse.toStyledString());
        res.end();
    }
    DBDisConnect(conn);
}

void DataManagementService::recoverCompany(const crow::request& req, crow::response& res) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;
    try {
        auto bodyInfo = crow::json::load(req.body);
        std::string email = bodyInfo["email"].s();

        try {
            sql::Statement *stmt = conn->createStatement();
            sql::ResultSet *queryResult = stmt->executeQuery("SELECT * FROM service.company_table WHERE email = '"
            + email + "'");

            if (queryResult->next()) {
                int company_id = queryResult->getInt("company_id");
                std::string company_name =  queryResult->getString("company_name");
                std::string jwtToken = generateJwtToken(company_id);
                CURL *curl = curl_easy_init();
                if (curl) {
                    const char *url = "https://api.sendgrid.com/v3/mail/send";
                    struct curl_slist* headers = NULL;
                    std::string auth = "Authorization: Bearer "+ DataManagementService::sendGrid_key;
                    headers = curl_slist_append(headers, auth.c_str());
                    headers = curl_slist_append(headers, "Content-Type: application/json");

                    // Set the JSON payload with email details
                    std::string jsonPayload = "{\"personalizations\":[{\"to\": [{\"email\":\"" + email + "\"}]}],\"from\": {\"email\":\"hl3608@columbia.edu\"},\"subject\": \"SubManager New Token\",\"content\": [{\"type\": \"text/plain\", \"value\": \"Your new token is: " + jwtToken + " \"}]}";  //NOLINT


                    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                    // verbose for debug
                    curl_easy_setopt(curl, CURLOPT_URL, url);
                    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

                    try {
                        CURLcode curl_res = curl_easy_perform(curl);
                        // Check the response
                        if (curl_res != CURLE_OK) {
                            res.code = 500;  // Internal Server Error
                            std::string errMsg(curl_easy_strerror(curl_res));
                            jsonResponse["error"] ="Failed to send email" +  errMsg;
                            res.write(jsonResponse.toStyledString());
                            res.end();
                            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_res));
                        } else {
                            res.code = 200;  // OK
                            jsonResponse["msg"] ="An email has been send to your address";
                            res.write(jsonResponse.toStyledString());
                            res.end();
                        }
                        // Clean up
                        curl_slist_free_all(headers);
                        curl_easy_cleanup(curl);
                    } catch (const std::exception &e) {
                        res.code = 500;  // Internal Server Error
                        jsonResponse["error"] ="Failed to send email" + std::string(e.what());
                        res.write(jsonResponse.toStyledString());
                        res.end();
                    }
                } else {
                    res.code = 500;  // Internal Server Error
                    jsonResponse["error"] ="Failed to send email with curl error";
                    res.write(jsonResponse.toStyledString());
                    res.end();
                }
            } else {
                res.code = 400;  // Bad Request
                jsonResponse["error"] ="Your email has not been registered";
                res.write(jsonResponse.toStyledString());
                res.end();
            }

            delete stmt;
            delete queryResult;
        } catch (sql::SQLException &e) {
            // Catch any SQL errors
            res.code = 500;  // Internal Server Error
            int errorCode = e.getErrorCode();
            if (errorCode == 1062) {
                // duplicate company email
                jsonResponse["error"] ="Add Company Error: You have already registered with this email,"
                " if you lost your JWT token, please apply for a new one.";
                res.write(jsonResponse.toStyledString());
            } else {
                jsonResponse["error"] ="Add Company Error: " + std::string(e.what());
                res.write(jsonResponse.toStyledString());
            }
            res.end();
        }
    } catch (const std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        res.write("Invalid request \n");
        jsonResponse["error"] ="Invalid request";
        res.write(jsonResponse.toStyledString());
        res.end();
    }

    DBDisConnect(conn);
}


void DataManagementService::addMember(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;
    if (companyId != -1) {
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
                jsonResponse["error"] = "Add Member Error: " + std::string(e.what());
                res.write(jsonResponse.toStyledString());
                res.end();
            }
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


std::string DataManagementService::memberLogin(const crow::request& req, crow::response& res, int companyId) {
    sql::Connection *conn = DBConnect();
    std::string sessionEmail = "";
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string email = bodyInfo["email"].s();
            std::string password = bodyInfo["password"].s();
            try {
                sql::Statement *stmt = conn->createStatement();
                std::string query = queryGenerator.searchMemeberForLogin(companyId, email, password);
                sql::ResultSet *queryResult = stmt->executeQuery(query);
                if (queryResult->next()) {
                    sessionEmail = email;
                    res.code = 200;
                    jsonResponse["msg"] = "success";
                    res.write(jsonResponse.toStyledString());
                } else {
                    res.code = 401;  // Unauthorized
                    jsonResponse["error"] = "Unauthorized";
                    res.write(jsonResponse.toStyledString());
                }
                delete queryResult;
                delete stmt;
                // res.end();
            }
            catch (sql::SQLException &e) {
                // Catch any SQL errors
                res.code = 500;  // Internal Server Error
                jsonResponse["error"] = "Login Error: " + std::string(e.what());
                res.write(jsonResponse.toStyledString());
                res.end();
            }
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

    if (companyId != -1) {
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
            jsonResponse["error"] = "Change Member Info Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::changeMemberInfo(const crow::request& req, crow::response& res, int companyId, std::string email) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
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
                jsonResponse["msg"] = "Update success";
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
                res.code = 400;   // Bad Request
                jsonResponse["error"] = "No Query Found To Update";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Change Member Info Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getMemberInfo(const crow::request& req, crow::response& res, int companyId, std::string email) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;
    if (companyId != -1) {
        try {
            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchMemeberByCompanyIdAndEmailQuery(companyId, email);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            
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
            jsonResponse["error"] = "Get Member Info Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
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
            } else {
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
    crow::response &res, int companyId, std::string email) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string subscriptionId = bodyInfo["subscription_id"].s();  // Required
            std::string subscriptionStatus = bodyInfo["subscription_status"].s();
            std::string billingInfo = bodyInfo["billing_info"].s();

            sql::Statement *searchStmt = conn->createStatement();
            std::string searchQuery = "SELECT * from service.subscription_table WHERE company_id = " + std::to_string(companyId) + " AND subscription_id = '"
            + subscriptionId + "' AND member_email = '" + email + "';";
            sql::ResultSet *queryResult = searchStmt->executeQuery(searchQuery);

            if (queryResult->rowsCount() > 0) {
                std::string query = queryGenerator.
                updateSubscriptionMember(subscriptionId, subscriptionStatus, billingInfo);
                sql::Statement *stmt = conn->createStatement();
                stmt->execute(query);
                res.code = 200;
                jsonResponse["msg"] = "Update Subscription Success";
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
                res.code = 400;
                jsonResponse["err"] = "No subscription found or you don't have permission to modify this subscription.";
                res.write(jsonResponse.toStyledString());
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
            jsonResponse["err"] = "Invalid request: " + std::string(e.what()) + "\n";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}


void DataManagementService::updateSubscriptionAction(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            auto bodyInfo = crow::json::load(req.body);
            std::string subscriptionId = bodyInfo["subscription_id"].s();  // Required
            std::string subscriptionAction = bodyInfo["last_action"].s();

            sql::Statement *searchStmt = conn->createStatement();
            std::string searchQuery = "SELECT * from service.subscription_table WHERE company_id = " + std::to_string(companyId) + " AND subscription_id = '" + subscriptionId + "';";
            sql::ResultSet *queryResult = searchStmt->executeQuery(searchQuery);

            if (queryResult->rowsCount() > 0) {
                std::string currentTime = "" + getCurrentDateTime();
                std::string query = queryGenerator.
                updateSubscriptionAction(subscriptionId, subscriptionAction, currentTime);
                sql::Statement *stmt = conn->createStatement();
                stmt->execute(query);
                res.code = 200;
                jsonResponse["msg"] = "Update Subscription Action Success";
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
                res.code = 400;
                jsonResponse["error"] = "No subscription found or you don't have permission to modify this subscription.";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Update Subscription Action Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid Request" + std::string(e.what()) + "\n";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::updateSubscriptionAdmin(const crow::request &req,
    crow::response &res, int companyId) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            std::cout << "function called";
            auto bodyInfo = crow::json::load(req.body);
            std::string subscriptionId = bodyInfo["subscription_id"].s();  // Required

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
                jsonResponse["msg"] = "Update Success";
                res.write(jsonResponse.toStyledString());
                res.end();
            } else {
                res.code = 400;
                jsonResponse["error"] = "No subscription found or you don't have permission to modify this subscription.";
                res.write(jsonResponse.toStyledString());
                res.end();
            }
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Update Subscription Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request \nWhat: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getCompanySubscriptions(const crow::request& req, crow::response& res, int companyId) {
    sql::Connection *conn = DBConnect();
    auto page_param = req.url_params.get("page");
    auto pageSize_param = req.url_params.get("pagesize");
    Json::Value jsonResponse;

    // Pagination dafualt setting
    int page = 1;
    int pageSize = 10;

    if (page_param != nullptr) {
        page = std::stoi(page_param);
    }
    if (pageSize_param != nullptr) {
        pageSize = std::stoi(pageSize_param);
    }

    try {
        // Get total number of subscriptions
        sql::Statement *countStmt = conn->createStatement();
        sql::ResultSet *countResult = countStmt->executeQuery("SELECT COUNT(*) as count FROM service.subscription_table WHERE company_id = '"
            + std::to_string(companyId) + "'");

        countResult->next();
        int totalSubscriptions = countResult->getInt("count");
        int totalPages = (totalSubscriptions + pageSize - 1)/pageSize;  // cround up to integer

        sql::Statement *stmt = conn->createStatement();
        sql::ResultSet *queryResult = stmt->executeQuery("SELECT * FROM service.subscription_table WHERE company_id = '"
            + std::to_string(companyId) + "'" + " LIMIT " + std::to_string(pageSize) + " OFFSET " + std::to_string((page-1)*pageSize));

        jsonResponse["total_subscriptions"] = std::to_string(totalSubscriptions);
        jsonResponse["total_pages"] = std::to_string(totalPages);
        Json::Value subsArray(Json::arrayValue);
        while (queryResult->next()) {
            Json::Value subsJson;
            subsJson["subscription_id"] = static_cast<std::string>(queryResult->getString("subscription_id"));
            subsJson["member_email"] = static_cast<std::string>(queryResult->getString("member_email"));
            subsJson["subscription_name"] = static_cast<std::string>(queryResult->getString("subscription_name"));
            subsJson["subscription_type"] = static_cast<std::string>(queryResult->getString("subscription_type"));
            subsJson["subscription_status"] = static_cast<std::string>(queryResult->getString("subscription_status"));
            subsJson["start_date"] = static_cast<std::string>(queryResult->getString("start_date"));
            subsJson["next_due_date"] = static_cast<std::string>(queryResult->getString("next_due_date"));
            subsJson["last_action"] = static_cast<std::string>(queryResult->getString("last_action"));
            subsJson["last_action_date"] = static_cast<std::string>(queryResult->getString("last_action_date"));
            subsJson["billing_info"] = static_cast<std::string>(queryResult->getString("billing_info"));

            subsArray.append(subsJson);
        }
        jsonResponse["subscriptions"] = subsArray;

        res.code = 200;
        res.add_header("Content-Type", "application/json");
        res.write(jsonResponse.toStyledString());
        res.end();
        delete queryResult;
        delete stmt;
    } catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Get Company All Subscriptions Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
    } catch (const std::exception &e) {
        // Catch invalid request errors
        res.code = 400;  // Bad Request
        jsonResponse["error"] = "Invalid request";
        res.write(jsonResponse.toStyledString());
        res.end();
    }

    DBDisConnect(conn);
}

void DataManagementService::viewSubscriptions(const crow::request& req,
crow::response& res, int companyId, bool isAdmin, std::string email) {
    sql::Connection *conn = DBConnect();
    Json::Value jsonResponse;

    if (companyId != -1) {
        try {
            if (isAdmin) {
                auto bodyInfo = crow::json::load(req.body);
                email = bodyInfo["email"].s();
            }

            sql::Statement *stmt = conn->createStatement();
            std::string query = queryGenerator.searchSubscriptioByCompanyIdAndEmailQuery(companyId, email);
            sql::ResultSet *queryResult = stmt->executeQuery(query);
            int total_subscriptions = static_cast<int>(queryResult->rowsCount());

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

                if (isAdmin) {
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
            jsonResponse["error"] = "View member's subscription Error: " + std::string(e.what()) ;
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
            res.end();
        }
    }
    DBDisConnect(conn);
}

void DataManagementService::getExpiringSubscriptionByTime(const crow::request &req,
    crow::response &res, int companyId) {
    Json::Value jsonResponse;
    sql::Connection *conn = DBConnect();

    if (companyId != -1) {
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
                    jsonResponse[std::to_string(counter)] = currEmail;
                    counter += 1;
                }
            }
            jsonResponse["number"] = std::to_string(counter);
            jsonResponse["target_time"] = targetTime;

            std::string jsonString = jsonResponse.toStyledString();
            res.code = 200;  // OK
            res.write(jsonString);
            res.end();
        }
        catch(const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Change Member Info Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            // Catch invalid request errors
            res.code = 400;  // Bad Request
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
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
                    if (curl) {
                        const char *url = "https://api.sendgrid.com/v3/mail/send";
                        struct curl_slist* headers = NULL;
                        std::string auth = "Authorization: Bearer "+ DataManagementService::sendGrid_key;
                        headers = curl_slist_append(headers, auth.c_str());
                        headers = curl_slist_append(headers, "Content-Type: application/json");

                        // Set the JSON payload with email details
                        std::string jsonPayload = "{\"personalizations\":[{\"to\": [{\"email\":\"" + email +
                        "\"}]}],\"from\": {\"email\":\"hl3608@columbia.edu\"},\"subject\": \"SubManager New Token\",\"content\": [{\"type\": \"text/plain\", \"value\": \"Your subscription is about to expire before " //NOLINT
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
    Json::Value jsonResponse;

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

            std::string analysis = analyze(durations, "Durations (day)");
            plot_hist(durations, "Durations (day)");
            send(email, "figure.pdf", analysis);

            res.code = 200;
            res.add_header("Content-Type", "application/json");
            jsonResponse["msg"] = "Analyze successfully";
            res.write(jsonResponse.toStyledString());
            res.end();
            delete queryResult;
            delete stmt;
        }
        catch (const sql::SQLException &e) {
            res.code = 500;
            jsonResponse["error"] = "Analyze Subscription Duration Error: " + std::string(e.what());
            res.write(jsonResponse.toStyledString());
            res.end();
        }
        catch (const std::exception &e) {
            res.code = 400;
            jsonResponse["error"] = "Invalid request";
            res.write(jsonResponse.toStyledString());
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
