/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include <jwt-cpp/jwt.h>
#include <string>
#include <chrono>  //NOLINT
#include <ctime>
#include <iomanip>
#include <iostream>
#include "./utils.hpp"

std::string Query::authenticationQuery(const std::string& username,
const std::string& password) {
    return "SELECT * FROM service.company_table WHERE company_name = '"
    + username + "' AND hash_pwd = '" + password + "'";
}

std::string Query::companyInfoQuery(int companyId) {
    return "SELECT * FROM service.company_table WHERE company_id = "
    + std::to_string(companyId);
}

std::string Query::searchMemeberByCompanyIdAndEmailQuery(int companyId, const std::string& email) {
    return "SELECT * FROM service.member_table WHERE company_id = "
    + std::to_string(companyId) + " AND email = '" + email + "';";
}

std::string Query::searchMemeberForLogin(int companyId, const std::string& email, const std::string& hash_pwd) {
    return "SELECT * FROM service.member_table WHERE company_id = "
    + std::to_string(companyId) + " AND email = '" + email + "' AND hash_pwd = '" + hash_pwd + "';";
}

std::string Query::deleteMemeberByCompanyIdAndEmailQuery(int companyId, const std::string& email) {
    return "DELETE FROM service.member_table WHERE company_id = "
    + std::to_string(companyId) + " AND email = '" + email + "';";
}

std::string Query::addCompanyInfoQuery(const std::string& email,
const std::string& companyName) {
    return "Insert into service.company_table (email, company_name) Values ('" + email
    + "', '" + companyName + "');";
}

std::string Query::updateCompanyInfoQuery(const std::string& companyName, int companyId) {
    return "UPDATE service.company_table SET company_name = '"+ companyName + "' WHERE company_id = " + std::to_string(companyId) + ";";
}

std::string Query::addMemberQuery(int companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& hash_password, const std::string& phoneNumber) {
    return "Insert into service.member_table (company_id, first_name, last_name, email, hash_pwd, phone_number) Values ("
    + std::to_string(companyId)  + ", '"
    + firstName + "', '" + lastName + "', '" + email + "', '" + hash_password + "', '" +
    phoneNumber + "');";
}

std::string Query::updateMemberInfoQuery(int companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& phoneNumber) {
    std::string query = "UPDATE service.member_table SET ";
    if (!firstName.empty()) {
        query += "first_name ='" + firstName + "', ";
    }
    if (!lastName.empty()) {
        query += "last_name = '"+ lastName + "', ";
    }
    if (!phoneNumber.empty()) {
        query += "phone_number = '"+ phoneNumber+ "', ";
    }

    query.pop_back();
    query.pop_back();  // remove trailing comma and space
    query += " WHERE company_id = "
    + std::to_string(companyId) + " AND email = '" + email + "';";
    return query;
}

std::string Query::addSubscriptionQuery(const std::string& memberEmail, int companyId, const std::string& subscriptionName,
    const std::string& subscriptionType, const std::string& subscriptionStatus, const std::string& nextDueDate,
    const std::string& startDate, const std::string& billingInfo) {
    return "Insert into service.subscription_table (member_email, company_id, subscription_name, subscription_type, subscription_status, next_due_date, start_date, billing_info) Values ('" + memberEmail + "', '" + std::to_string(companyId) + "', '" +  //NOLINT
    subscriptionName + "', '" + subscriptionType + "', '" + subscriptionStatus + "', '" +
    nextDueDate + "', '" + startDate + "', '" + billingInfo + "');";
    }

std::string Query::searchSubscriptioByCompanyIdAndEmailQuery(int companyId, const std::string& email) {
    return "SELECT * FROM service.subscription_table WHERE company_id = "
    + std::to_string(companyId) + " AND member_email = '" + email + "';";
}

std::string Query::searchSubscriptioByCompanyIdAndEmailAndSubscriptionNameQuery(int companyId, const std::string& email, const std::string& subscriptionName) {
    return "SELECT * FROM service.subscription_table WHERE company_id = "
    + std::to_string(companyId) + " AND member_email = '" + email + "' AND subscription_name = '" + subscriptionName + "';";
}

std::string Query::updateSubscriptionAction(const std::string& subscription_id,
    const std::string& lastAction, const std::string& currentTime) {
    return "UPDATE service.subscription_table SET last_action = '"
    + lastAction + "', last_action_date = '" + currentTime + "' WHERE subscription_id = '" + subscription_id + "';";
}

std::string Query::updateSubscriptionAdmin(const std::string& subscription_id,
const std::string& subscription_name, const std::string& subscription_type,
const std::string& subscription_status, const std::string& start_date, const std::string& next_due_date,  const std::string& billing_info) {
    std::string query = "UPDATE service.subscription_table SET subscription_name ='" + subscription_name
    + "', subscription_type = '"+ subscription_type + "', subscription_status = '"+ subscription_status
    + "', start_date = '"+ start_date + "', next_due_date = '"+ next_due_date + "', billing_info = '"+ billing_info + "' WHERE subscription_id = '" + subscription_id + "';";
    return query;
}

std::string Query::updateSubscriptionMember(const std::string& subscription_id, const std::string& subscription_status, const std::string& billing_info) {
    std::string query = "UPDATE service.subscription_table SET subscription_status = '"+ subscription_status
    + "', billing_info = '"+ billing_info + "' WHERE subscription_id = '" + subscription_id + "';";
    return query;
}

std::string Query::searchFutureExpireSubscriptioByCompanyIdAndEmailAndrangeDaysAndsubscriptionId
(int companyId, const std::string&  targetTime, const std::string&  subscriptionName) {
    std::string queryStmt = "SELECT member_email FROM service.subscription_table WHERE company_id = "
    + std::to_string(companyId) + " AND next_due_date <= '" + targetTime + "' ";
    if (subscriptionName.size() != 0) {
        queryStmt += "AND subscription_name = '" + subscriptionName + "'";
    }
    return queryStmt + ";";
}

std::string Query::searchSubscriptionDurationByCompanyId(int companyId) {
    return "SELECT DATEDIFF(next_due_date, start_date) AS duration FROM service.subscription_table WHERE company_id = " + std::to_string(companyId) + ";";
}

std::string getCurrentDateTime() {
    auto current = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(current);
    std::stringstream res;
    res << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    return res.str();
}

std::string timeAddition(int numDays) {
    auto current = std::chrono::system_clock::now();
    std::chrono::hours hoursToAdd(numDays*24);
    std::chrono::system_clock::time_point target = current + hoursToAdd;
    std::time_t targetTime = std::chrono::system_clock::to_time_t(target);
    std::stringstream res;
    res << std::put_time(std::localtime(&targetTime), "%Y-%m-%d %H:%M:%S");
    return res.str();
}


