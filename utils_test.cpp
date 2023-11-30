/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#define CATCH_CONFIG_MAIN

#include <string>
#include <./catch2/catch.hpp>
#include "./utils.hpp"
#include <jwt-cpp/jwt.h>


Query queryGenerator;

TEST_CASE("Authentication Query", "[single-file]" ) {
    std::string query =
        queryGenerator.authenticationQuery("username", "password");
    std::string target_query = "SELECT * FROM service.company_table WHERE company_name = 'username' AND hash_pwd = 'password'";
    REQUIRE(query == target_query);
}

TEST_CASE("Company Info Query", "[single-file]" ) {
    std::string query = queryGenerator.companyInfoQuery(1);
    std::string target_query = "SELECT * FROM service.company_table WHERE company_id = 1";
    REQUIRE(query == target_query);
}

TEST_CASE("Add Company Info Query", "[single-file]" ) {
    std::string query = queryGenerator.addCompanyInfoQuery("email", "companyName");
    std::string target_query = "Insert into service.company_table (email, company_name) Values ('email', 'companyName');";
    REQUIRE(query == target_query);
}

TEST_CASE("Add Member Query", "[single-file]" ) {
    std::string query = queryGenerator.addMemberQuery("1", "firstName", "lastName", "email", "abc12345", "1111111111");
    std::string target_query = "Insert into service.member_table (company_id, first_name, last_name, email, hash_pwd, phone_number) Values (1, 'firstName', 'lastName', 'email', 'abc12345', '1111111111');";
    REQUIRE(query == target_query);
}
