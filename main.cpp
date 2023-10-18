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


    /**
     * Get a company's information
     * http://localhost:3000/company?username=company1&password=pwd
    */
    CROW_ROUTE(app, "/company")
        .methods(crow::HTTPMethod::GET)
    ([&] (const crow::request& req, crow::response& res)
    {
        sql::Connection* conn = DBConnect();
        int companyId = isUserAuthenticated(req, res, conn);

        std::cout << "company Id is: " << companyId << std::endl;
        if(companyId != -1){
            sql::PreparedStatement* prep_stmt = conn->prepareStatement("SELECT * FROM service.company_table WHERE company_id = ?");
            prep_stmt->setInt(1,companyId);

            try{
                sql::ResultSet* queryResult = prep_stmt->executeQuery();
                if(queryResult->rowsCount() > 0){
                    std::string companyData= ""; 
                    while( queryResult->next()){
                        std::cout << "queryFounded";
                        companyData += queryResult->getInt("company_id");
                        //companyData += queryResult->getString("company_name") + ", ";
                        //companyData += queryResult->getString("email") + " \n";
                    }

                    std::cout << "print company data: " << companyData;
                    res.code = 200;
                    res.write(companyData);
                }else{
                    std::cout << "noqueryFound" << std::endl;
                }
                
                delete queryResult;
                delete prep_stmt;

            } catch (const sql::SQLException& e){
                std::cerr << "SQL ERROR: " << e.what() << std::endl;
            }
        }
        DBDisConnect(conn);
    });
    
    app.port(3000).multithreaded().run();
}