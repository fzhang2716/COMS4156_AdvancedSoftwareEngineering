/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#define CATCH_CONFIG_MAIN

#include <string>
#include <./catch2/catch.hpp>
#include "./utils.hpp"


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
    std::string query = queryGenerator.addCompanyInfoQuery("1",
    "email", "hashPwd", "companyName");
    std::string target_query = "Insert into service.company_table Values (1, 'companyName', 'email', 'hashPwd');";
    REQUIRE(query == target_query);
}

TEST_CASE("Add Member Query", "[single-file]" ) {
    std::string query = queryGenerator.addMemberQuery("1", "firstName", "lastName", "email", "1111111111", "gold");
    std::string target_query = "Insert into service.member_table Values (1, 'firstName', 'lastName', 'email', '1111111111', 'gold');";
    REQUIRE(query == target_query);
}
