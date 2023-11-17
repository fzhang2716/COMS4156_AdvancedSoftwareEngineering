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

std::string Query::addCompanyInfoQuery(const std::string& email,
const std::string& companyName) {
    return "Insert into service.company_table (email, company_name) Values ('" + email
    + "', '" + companyName + "');";
}

std::string Query::addMemberQuery(const std::string& companyId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& phoneNumber,
const std::string& memberStatus) {
    return "Insert into service.member_table (company_id, first_name, last_name, email, phone_number, member_status) Values (" + companyId  + ", '"
    + firstName + "', '" + lastName + "', '" + email + "', '" +
    phoneNumber + "', '"+ memberStatus + "');";
}
