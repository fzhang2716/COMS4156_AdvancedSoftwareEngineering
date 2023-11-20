/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include <string>
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

std::string Query::updateCompanyInfoQuery(const std::string& email,
const std::string& companyName, int companyId) {
    return "UPDATE service.company_table SET email ='" + email
    + "'," + "company_name = '"+ companyName + "' WHERE company_id = " + std::to_string(companyId) + ";";
}

std::string Query::addMemberQuery(const std::string& companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& phoneNumber) {
    return "Insert into service.member_table (company_id, first_name, last_name, email, phone_number) Values ("
    + companyId  + ", '"
    + firstName + "', '" + lastName + "', '" + email + "', '" +
    phoneNumber + "');";
}

std::string Query::updateMemberInfoQuery(const std::string& companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& phoneNumber) {
    return "UPDATE service.member_table SET first_name ='" + firstName
    + "'," + "last_name = '"+ lastName + "'," + "phone_number = '"+ phoneNumber+ "' WHERE company_id = "
    + companyId + " AND email = '" + email + "';";
}
