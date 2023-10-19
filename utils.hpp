#include <string>

class Query
{
public:
    std::string authenticationQuery(const std::string& username, const std::string& password);
    std::string companyInfoQuery(int companyId);
    std::string addCompanyInfoQuery(const std::string& companyId, const std::string& email, const std::string& hashPwd, const std::string& companyName);
    std::string addMemberQuery(const std::string& memberId, const std::string& firstName, const std::string& lastName, const std::string& email, const std::string& phoneNumber, const std::string& memberStatus);
};
