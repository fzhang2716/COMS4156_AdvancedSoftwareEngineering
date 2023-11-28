/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include <string>
#include <chrono>
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

std::string Query::addMemberQuery(const std::string& companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& hash_password, const std::string& phoneNumber) {
    return "Insert into service.member_table (company_id, first_name, last_name, email, hash_pwd, phone_number) Values ("
    + companyId  + ", '"
    + firstName + "', '" + lastName + "', '" + email + "', '" + hash_password + "', '" +
    phoneNumber + "');";
}

std::string Query::updateMemberInfoQuery(const std::string& companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& phoneNumber) {
    std::string query = "UPDATE service.member_table SET ";
    if(!firstName.empty()){
        query += "first_name ='" + firstName + "', ";
    }
    if(!lastName.empty()){
        query += "last_name = '"+ lastName + "', ";
    }
    if(!phoneNumber.empty()){
        query += "phone_number = '"+ phoneNumber+ "', ";
    }

    query.pop_back();
    query.pop_back(); // remove trailing comma and space
    query += " WHERE company_id = "
    + companyId + " AND email = '" + email + "';";
    return query;
}

std::string Query::addSubscriptionQuery(const std::string& memberEmail, int companyId, const std::string& subscriptionName, 
    const std::string& subscriptionType, const std::string& subscriptionStatus, const std::string& nextDueDate, 
    const std::string& startDate, const std::string& billingInfo){
       
    return "Insert into service.subscription_table (member_email, company_id, subscription_name, subscription_type, subscription_status, next_due_date, start_date, billing_info) Values ('" + memberEmail + "', '" + std::to_string(companyId) + "', '" +
    subscriptionName + "', '" + subscriptionType + "', '" + subscriptionStatus + "', '" +
    nextDueDate + "', '" + startDate + "', '" + billingInfo + "');";
    }

std::string Query::searchSubscriptioByCompanyIdAndEmailAndSubscriptionNameQuery(int companyId, const std::string& email, const std::string& subscriptionName) {
    return "SELECT * FROM service.subscription_table WHERE company_id = "
    + std::to_string(companyId) + " AND member_email = '" + email + "' AND subscription_name = '" + subscriptionName + "';";
}

std::string Query::updateSubscriptionAction(int companyId, const std::string& email,
    const std::string& subscriptionName, const std::string& subscriptionStatus,
    const std::string& currentTime, const std::string& newAction){
    return "UPDATE service.subscription_table SET last_action = '"
    + subscriptionStatus + "', last_action_date = '" + currentTime + "', subscription_status = '" + newAction + "' WHERE company_id = " + std::to_string(companyId) + " AND member_email = '" + email + + "' AND subscription_name = '" + subscriptionName + "';";
}



std::string getCurrentDateTime() {
    auto current = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(current);
    std::stringstream res;
    res << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    return res.str();
}

