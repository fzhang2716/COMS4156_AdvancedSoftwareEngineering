/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#ifndef UTILS_HPP_
#define UTILS_HPP_
#include <string>

class Query {
public:
    std::string authenticationQuery(const std::string& username, const std::string& password);
    std::string companyInfoQuery(int companyId);
    std::string addCompanyInfoQuery(const std::string& email, const std::string& companyName);
    std::string updateCompanyInfoQuery(const std::string& email, const std::string& companyName, int companyId);
    std::string addMemberQuery(const std::string& memberId, const std::string& firstName, const std::string& lastName, const std::string& email, const std::string& phoneNumber, const std::string& memberStatus);
};

#endif //  UTILS_HPP_