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

    // need to implement
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


    /**
     * Add a company's infomation
     * Example request: http://localhost:3000/addCompany?company_id=8&email=email8&hash_pwd=12321&company_name=company8
     */
    CROW_ROUTE(app, "/addCompany")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        sql::Connection* conn = DBConnect();
        addCompany(req, conn, res);

        res.end();
        DBDisConnect(conn);
    });

    
    // Add a subscription's infomation
    // Example request: http://localhost:3000/addSubscription
    // {
    // "subscription_id": "6",
    // "member_id": "3",
    // "company_id": "5",
    // "subscription_type": "Free",
    // "subscription_status": "active",
    // "next_due_date": "2023-10-29 00:00:00",
    // "start_date": "2023-9-17 00:00:00",
    // "billing_info": "infomation"
    // }

    CROW_ROUTE(app, "/addSubscription")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        sql::Connection* conn = DBConnect();
        addSubscription(req, conn, res);

        res.end();
        DBDisConnect(conn);
    });
        
    // Add a memeber's infomation
    // Example request: http://localhost:3000/addMemeber
    // {
    // "member_id": "5",
    // "first_name": "mike",
    // "last_name": "hughs",
    // "email": "m@gmail.com",
    // "phone_number": "123213",
    // "member_status": "gold"
    // }
    CROW_ROUTE(app, "/addMemeber")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req, crow::response &res){
        sql::Connection* conn = DBConnect();
        addMember(req, conn, res);

        res.end();
        DBDisConnect(conn);
    });

    app.port(3000).multithreaded().run();
}