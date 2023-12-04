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
    std::string searchMemeberForLogin(int companyId, const std::string& email, const std::string& hash_pwd);
    std::string deleteMemeberByCompanyIdAndEmailQuery(int companyId, const std::string& email);
    std::string addCompanyInfoQuery(const std::string& email, const std::string& companyName);
    std::string updateCompanyInfoQuery(const std::string& companyName, int companyId);
    std::string addMemberQuery(const std::string& companyId, const std::string& firstName, const std::string& lastName,
    const std::string& email, const std::string& hash_password, const std::string& phoneNumber);
    std::string updateMemberInfoQuery(const std::string& companyId, const std::string& firstName,
    const std::string& lastName, const std::string& email, const std::string& phoneNumber);
    std::string addSubscriptionQuery(const std::string& memberEmail, int companyId, const std::string& subscriptionType,
    const std::string& subscriptionName, const std::string& subscriptionStatus, const std::string& nextDueDate,
    const std::string& startDate, const std::string& billingInfo);
    std::string searchSubscriptioByCompanyIdAndEmailQuery(int companyId, const std::string& email);
    std::string searchSubscriptioByCompanyIdAndEmailAndSubscriptionNameQuery(int companyId,
    const std::string& email, const std::string& subscriptionName);
    std::string updateSubscriptionAction(int companyId, const std::string& email,
    const std::string& subscriptionName, const std::string& subscriptionStatus,
    const std::string& currentTime, const std::string& newAction);
    std::string searchFutureExpireSubscriptioByCompanyIdAndEmailAndrangeDaysAndsubscriptionId
    (int companyId, const std::string&  targetTime, const std::string& subscriptionName);

};

std::string getCurrentDateTime();
std::string timeAddition(int numDays);

#endif  //  UTILS_HPP
