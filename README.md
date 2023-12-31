# COMS4156_AdvancedSoftwareEngineering
## Team
**Name:** DebugTeam

**Members:** Frank Zhang (hz2716), Han Liu (hl3608), Xinghang Sun (xs2421), Yuxuan Chen (yc4144)

## Overview
Our project builds a subscription management service that provides sub-services like subscription data management, analytics, insights feedback, notification sending, alerting, and reports generation.

Our service is a subscription management service that can help a company to manage their members and their member’s subscriptions to the company’s services. In addition, our service provides analysis and alerting features that can help a company get insights into their subscription statistics and notify their members whose subscriptions are about to be due.

## Client App Demo
https://github.com/everssun/4156_final_project_client_app

## Install (Linux)
### [Crow](https://crowcpp.org/master/getting_started/setup/linux/)
( C++ framework for creating HTTP or Websocket web services )

 ```
 git clone https://github.com/CrowCpp/Crow.git
 cd Crow
 mkdir build
 sudo apt install libasio-dev
 cd build
 cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
 sudo make install
 ```

### [MySQL Connector/C++](https://dev.mysql.com/doc/dev/connector-cpp/latest/)
( C++ library for applications to communicate with MySQL database servers )

```
sudo apt update
sudo apt-get install libmysqlcppconn-dev
```


### [Catch2](https://github.com/catchorg/Catch2)
( A unit testing framework for C++)
 ```
 git clone -b v2.x https://github.com/catchorg/Catch2.git
 cd Catch2
 cmake -Bbuild -H. -DBUILD_TESTING=OFF
 sudo cmake --build build/ --target install
 ```

### [jwt-cpp](https://github.com/Thalhammer/jwt-cpp)
(A header only library for creating and validating JSON Web Tokens in C++11)
```
git clone https://github.com/Thalhammer/jwt-cpp.git
```

### [cURL](https://curl.se/)

```
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev
```

### [JSONCPP](https://github.com/open-source-parsers/jsoncpp)
```
sudo apt-get install libjsoncpp-dev
```
### [cpp-httplib](https://github.com/yhirose/cpp-httplib)
```
git clone https://github.com/yhirose/cpp-httplib.git
```
### [matplotlib-cpp](https://github.com/lava/matplotlib-cpp)
Need to install Python3.8 first
```
sudo apt-get install python3-matplotlib python3-numpy python3.8-dev
```
## Third-party APIs
### [SendGrid](https://sendgrid.com/en-us/solutions/email-api)
(email services)

Need to add the api keys:

*data_management.hpp* [#Line27](https://github.com/fzhang2716/COMS4156_AdvancedSoftwareEngineering/blob/44aaddd8637b95ae7e7029714efbf2b567c06fd6/data_management.hpp#L27) <{API_KEY}>

*send_attachment.cpp* [#Line100](https://github.com/fzhang2716/COMS4156_AdvancedSoftwareEngineering/blob/44aaddd8637b95ae7e7029714efbf2b567c06fd6/send_attachment.cpp#L100) <"Authorization: Bearer "+ {API_KEY}>

## Compile and Run
In root directory, run Makefile:

```
make
./main
```

## Unit Test
### utilis test
In root directory, run Makefile:
```
make utils_test
./utils_test
```

Result:
```
===============================================================================
All tests passed (4 assertions in 4 test cases)
```

### analyze data test
In root directory, run Makefile:
```
make analyze_data_test
./analyze_data_test
```

Result:
```
===============================================================================
All tests passed (12 assertions in 6 test cases)
```

## Integration Test
In root directory, run Makefile:
```
make integration_test
./integration_test
```

Result:
```
===============================================================================
All tests passed (24 assertions in 10 test cases)
```
## Static Analysis Tool
We use Cppcheck, which is a static analysis tool for C/C++ code. It provides unique code analysis to detect bugs and focuses on detecting undefined behaviour and dangerous coding constructs. 
```
sudo apt-get install cppcheck
cppcheck *.cpp --force --output-file=cppcheck-report.txt
```
We include such static analysis in our CI.

## Style Check
We use Cpplint, which is a command-line tool to check C/C++ files for style issues following Google's C++ style guide.
```
pip install cpplint
cpplint --linelength=200 --filter=-runtime/references [filename]
```

## REST API

Refer to this [Postman Workspace](https://app.getpostman.com/join-team?invite_code=e99d0f0ace00953a60793b4fba6269fb&target_code=ba804970dfad862285f0124ba74c65c2) or [Google Doc Screenshots](https://docs.google.com/document/d/1liqiSBghhSN7_ZPRPIBlw0qJ3BbGde0nJYx_kBdFqIo/edit?usp=sharing) to see example API requests.

Refer to this [API Curl Example](API.md) to see example API requests with curl.

> [!NOTE]  
> Our service's target clients are any SAAS providers or entities who provide subscriptions to their members. Our clients are referred to as "Company", and their members are referred to as "members". To use our service, clients (a.k.a companies) need to send a request to the "/addCompany" endpoint for a JWT token. JWT token authorization is used in most APIs. Note that our service provides some unprotected endpoints for the company's admin and some login cookie-protected endpoints for the company's members.

### - Register as a new company:
- **Endpoint:** `/company/addCompany`
- **Method:** POST
- **Request Body:** JSON {"email", "company_name"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg" : "Add Company Success", "tokenMsg": "<JWT_token>"}
- **Error Response:** 
    - HTTP 500 Internal Server Error, with an error message [e.g. registered email]
    - HTTP 400 Bad Request, with an error message [e.g. invalid request body]

### - Get company's info:
- **Endpoint:** `/company`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Success Response:** HTTP 200 OK, with a JSON {"company_id", "company_name", "email"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Company re-apply for the access token:
- **Endpoint:** `/recoverCompany`
- **Method:** POST
- **Request Body:** JSON {"email"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "An email has been send to your address "}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. Email has not been registered]
    - HTTP 500 Internal Server Error, with an error message

### - Update company's info:
- **Endpoint:** `/company/changeCompany`
- **Method:** PATCH
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"company_name"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Update Company Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Add a new member:
- **Endpoint:** `/member/addMember`
- **Method:** POST
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"first_name", "last_name", "email", "password", "phone_number"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Add Member Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message [e.g. duplicate email]

### - Get company's all members (pagination support):
- **Endpoint:** `/company/getMembers?page=<int>&pagesize=<int>` (parameters are optional, defualt page=1, pageSize=10)
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Success Response:** HTTP 200 OK, with a JSON {"total_members", "total_pages", "members": [a JSON array of {"email", "first_name", "last_name", "phone_number"}]}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message [e.g. duplicate email]

### - Delete a member as admin:
- **Endpoint:** `/admin/member/removeMember/<string:memberEmail>`
- **Method:** DELETE
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Success Response:** HTTP 204 No Content, with a JSON {"msg": "Delete Member Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. no matching member found]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Change member's info as admin:
- **Endpoint:** `/admin/member/changeMemberInfo`
- **Method:** PATCH
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"first_name", "last_name", "email", "phone_number"} 
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Update success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. member email not exists]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Member login:
- **Endpoint:** `/member/login`
- **Method:** POST
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"email", "password"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "success"} and a message "Login Successfully"
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token" or member email and password not matching
    - HTTP 500 Internal Server Error, with an error message

### - Change member's info by self:
- **Endpoint:** `/member/changeMemberInfo`
- **Method:** PATCH
- **Request Header:** 'Authorization: Bearer {JWT token}', 'Cookie: {cookie get after member loged in}'
- **Request Body:** JSON {"first_name", "last_name", "phone_number"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Update success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. member not loged in]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Member logout:
- **Endpoint:** `/member/logout`
- **Method:** POST
- **Request Header:** 'Authorization: Bearer {JWT token}', 'Cookie: {cookie get after member loged in}'
- **Success Response:** HTTP 200 OK, with a message "Logout Successfully"
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. member not loged in]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Get member's info by self:
- **Endpoint:** `/member/profile`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}', 'Cookie: {cookie get after member loged in}'
- **Success Response:** HTTP 200 OK, with a JSON {"email", "first_name", "last_name", "phone_number"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. member not loged in]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Get member's info as admin:
- **Endpoint:** `/admin/member/profile/<string:memberEmail>`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Success Response:** HTTP 200 OK, with a JSON {"email", "first_name", "last_name", "phone_number"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. no matching member email]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Add a new subscription:
- **Endpoint:** `/subscription/addSubscription`
- **Method:** POST
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"member_email", "subscription_name", "subscription_type", "subscription_status", "next_due_date", "start_date", "billing_info"}
- **Success Response:** HTTP 200 OK, with a JSON  {"msg":"Add Subscription Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Update subscription by self:
- **Endpoint:** `/subscription/updateSubscription`
- **Method:** PATCH
- **Request Header:** 'Authorization: Bearer {JWT token}','Cookie: {cookie get after member loged in}'
- **Request Body:** JSON {"subscription_id", "subscription_status", "billing_info"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Update Subscription Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. member not loged in]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Update subscription as admin:
- **Endpoint:** `/admin/subscription/updateSubscription`
- **Method:** PATCH
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"subscription_id", "subscription_name", "subscription_type", "subscription_status", "start_date", "next_due_date", "billing_info"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Update Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Update subscription modify action:
- **Endpoint:** `/subscription/updateSubscriptionAction`
- **Method:** PATCH
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"subscription_id", "last_action"}
- **Success Response:** HTTP 200 OK, with a JSON {"msg":"Update Subscription Action Success"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - View a member's subscriptions as admin:
- **Endpoint:** `/admin/subscription/viewSubscriptions`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"email"}
- **Success Response:** HTTP 200 OK, with a JSON {"total_subscriptions", "subscriptions": [a JSON array of {"subscription_id", "subscription_name", "subscription_type", "subscription_status", "billing_info", "start_date", "next_due_date", "last_action", "last_action_date"}]}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - View a member's subscriptions by self:
- **Endpoint:** `/subscription/viewSubscriptions`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}','Cookie: {cookie get after member loged in}'
- **Success Response:** HTTP 200 OK, with a JSON {"total_subscriptions", "subscriptions": [a JSON array of {"subscription_id", "subscription_name", "subscription_type", "subscription_status", "billing_info", "start_date", "next_due_date"}]}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message [e.g. member not loged in]
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Get all subscriptions of a company (pagination support):
- **Endpoint:** `/subscription/allSubscriptions?page=<int>&pagesize=<int>`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Success Response:** HTTP 200 OK, with a JSON {"total_subscriptions", "total_pages", "subscriptions": [a JSON array of {"subscription_id", "subscription_name", "subscription_type", "subscription_status", "billing_info", "start_date", "next_due_date", "last_action", "last_action_date"}]}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message


### - Get expiring subscriptions of a company:
- **Endpoint:** `/company/getExpiringSubscriptionByTime??subscription_name=<string: subscription_name>&days=<int>`
- **Method:** GET
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Success Response:** HTTP 200 OK, with a JSON {"target_time", "number", "0" : <first member's email>, "1": <second member's email>, ...}.
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Send email reminders to members:
- **Endpoint:** `/company/sendReminder`
- **Method:** POST
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"number", "0", "1", "target_time"}, the same format as success response of *Get expiring subscriptions of a company*
- **Success Response:** HTTP 200 OK, with a JSON {"msg": "Send successfully"}.
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message

### - Send company analysis report of subscription duration:
- **Endpoint:** `/company/analyzeSubDuration`
- **Method:** POST
- **Request Header:** 'Authorization: Bearer {JWT token}'
- **Request Body:** JSON {"email"}
- **Success Response:** HTTP 200 OK, with a JSON {""msg": Analyze successfully"}
- **Error Response:**
    - HTTP 400 Bad Request, with an error message
    - HTTP 401 Unauthorized, with an error message "JWT token not found" or "Invalid Token"
    - HTTP 500 Internal Server Error, with an error message
