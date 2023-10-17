#include "crow.h"

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
    crow::json::wvalue x({{"message", "Hello, World!"}});
    x["message2"] = "Hello, World.. Again!";
    return x; });

    app.port(3000).multithreaded().run();
}