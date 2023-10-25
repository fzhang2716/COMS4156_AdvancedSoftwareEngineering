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

std::string Query::addCompanyInfoQuery(const std::string& companyId,
const std::string& email, const std::string& hashPwd,
const std::string& companyName) {
    return "Insert into service.company_table Values (" + companyId
    + ", '" + companyName + "', '" + email + "', '" + hashPwd + "');";
}

std::string Query::addMemberQuery(const std::string& memberId,
const std::string& firstName, const std::string& lastName,
const std::string& email, const std::string& phoneNumber,
const std::string& memberStatus) {
    return "Insert into service.member_table Values (" + memberId  + ", '"
    + firstName + "', '" + lastName + "', '" + email + "', '" +
    phoneNumber + "', '"+ memberStatus + "');";
}
