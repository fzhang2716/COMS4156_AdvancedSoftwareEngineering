// Sets up the server
#include<iostream>
#include "data_management.hpp"
#include "crow.h"

int main() {
    DataManagementService datamanage;
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
    return 0;
}