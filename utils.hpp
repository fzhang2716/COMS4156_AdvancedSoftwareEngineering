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
    std::string searchMemeberByCompanyIdAndEmailQuery(int companyId, const std::string& email);
    std::string deleteMemeberByCompanyIdAndEmailQuery(int companyId, const std::string& email);
    std::string addCompanyInfoQuery(const std::string& email, const std::string& companyName);
    std::string updateCompanyInfoQuery(const std::string& companyName, int companyId);
    std::string addMemberQuery(const std::string& memberId, const std::string& firstName, const std::string& lastName,
    const std::string& email, const std::string& phoneNumber);
    std::string updateMemberInfoQuery(const std::string& companyId, const std::string& firstName,
    const std::string& lastName, const std::string& email, const std::string& phoneNumber);
    std::string searchSubscriptioByCompanyIdAndEmailAndSubscriptionNameQuery(int companyId,
    const std::string& email, const std::string& subscriptionName);
    std::string updateSubscriptionAction(int companyId, const std::string& email,
    const std::string& subscriptionName, const std::string& subscriptionStatus,
    const std::string& currentTime, const std::string& newAction);

};

std::string getCurrentDateTime();

#endif  //  UTILS_HPP
