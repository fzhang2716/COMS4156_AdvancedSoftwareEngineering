#include "crow.h"
#include "database.cpp"

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/test")
    ([]()
     {
        // result of a function
        return "somthing"; });

    CROW_ROUTE(app, "/json")
    ([]
     {
    sql::Connection* conn = DBConnect();
    crow::json::wvalue x({{"message", GetTestTable(conn)}});
    DBDisConnect(conn);
    return x; });

    // CROW_ROUTE(app, "/check/SubscriptionbyCompany")
    //     .methods(crow::HTTPMethod::GET)
    // ([&] (const crow::request& req, crow::response& res)
    // {
    //     sql::Connection* conn = DBConnect();
    //     // User Authentication
    //     // std::string checkResult = getSubscriptionByCompany(req, res, conn);

    //     DBDisConnect(conn);
    //     return "None";
    // });

    /**
     * Get a company's information
     * Example request: http://localhost:3000/company?username=company1&password=pwd
     */
    CROW_ROUTE(app, "/company")
        .methods(crow::HTTPMethod::GET)([&](const crow::request &req, crow::response &res)
                                        {
        sql::Connection* conn = DBConnect();
        // User Authentication
        int companyId = isUserAuthenticated(req, res, conn);

        if(companyId != -1){
            sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM service.company_table WHERE company_id = ?");
            prep_stmt->setInt(1,companyId);

            try{
                sql::ResultSet* queryResult = prep_stmt->executeQuery();
                if(queryResult->rowsCount() > 0){
                    std::string companyData= ""; 
                    while( queryResult->next()){
                        companyData += "Company ID: " + queryResult->getString("company_id") + "; ";
                        companyData += "Company Name: " + queryResult->getString("company_name") + "; ";
                        companyData += "Company email: " + queryResult->getString("email") + " \n";
                    }
                    res.code = 200;
                    res.write("Result: " + companyData);
                    res.end();
                }else{
                    res.code = 200;
                    res.write("No Query Found");
                    res.end();
                }
                
                delete queryResult;
                delete prep_stmt;
            } catch (const sql::SQLException& e){
                res.code = 500;
                res.write("Database Error: " + std::string(e.what()) + "\n");
                res.end();
            }
        }
        DBDisConnect(conn); });

    CROW_ROUTE(app, "/addCompany")
    .methods("POST"_method)
    ([](const crow::request& req){
        sql::Connection* conn = DBConnect();
        std::string res = addCompany(req, conn);

        return res;
    });

    app.port(3000).multithreaded().run();
}