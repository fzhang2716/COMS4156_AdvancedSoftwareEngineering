# COMS4156_AdvancedSoftwareEngineering

## Overview
Our project is a subscription management service that provides sub-services like subscription data management, analytics, insights feedback, notification sending, alerting, and reports generation.

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
Need to install Python3.9 first
```
sudo apt-get install python3-matplotlib python3-numpy python3.9-dev
```

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
All tests passed (2 assertions in 1 test case)
```

## Style Check
We use Cpplint, which is a command-line tool to check C/C++ files for style issues following Google's C++ style guide.
```
pip install cpplint
cpplint [filename]
```

## REST API

Refer to this [Postman Workspace](https://app.getpostman.com/join-team?invite_code=e99d0f0ace00953a60793b4fba6269fb&target_code=ba804970dfad862285f0124ba74c65c2) to see example API requests.

For the following request, if the client did not provide correct request syntax, for example, missing parameters, then the server responses "400 Bad Request" like this:

```
HTTP/1.1 400 Bad Request
Content-Length: 17
Server: Crow/master
Date: Thu, 19 Oct 2023 21:39:58 GMT
Connection: Keep-Alive

Invalid request 
```

If the client did not provide JWT token for certain requests, the server responses "401 Unauthorized" like this:
```
HTTP/1.1 401 Unauthorized
Content-Length: 21
Server: Crow/master
Date: Fri, 01 Dec 2023 03:41:01 GMT
Connection: Keep-Alive

JWT token not found 
``` 

If the client provided invalid JWT token for certain requests, the server responses "401 Unauthorized" like this:
```
HTTP/1.1 401 Unauthorized
Content-Length: 15
Server: Crow/master
Date: Fri, 01 Dec 2023 03:40:03 GMT
Connection: Keep-Alive

Invalid token 
```


### Register as a new client (company):

#### Request
`POST /addCompany`

```
curl -i --location 'http://localhost:3000/addCompany' \
--header 'Content-Type: text/plain' \
--data-raw '{
    "email" : "eg@gmail.com",
    "company_name" : "EG"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 195
Server: Crow/master
Date: Fri, 03 Nov 2023 04:10:27 GMT
Connection: Keep-Alive

Add Company Success 
Please save your JWT token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzA1MjA2MjgsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIxNiJ9.kfC9TVEi8AgCLSrXF8obpQLi76gjXhdvtP5M6_kJaXU
```

Failed (company email has been registered):

e.g. duplicate id
```
HTTP/1.1 500 Internal Server Error
Content-Length: 119
Server: Crow/master
Date: Fri, 03 Nov 2023 04:10:45 GMT
Connection: Keep-Alive

Add Company Error: You have already registered with this email, if you lost your JWT token, please apply for a new one.
```

### Get client's company info:

#### Request
`GET /company` 

```
curl -i --location 'http://localhost:3000/company' \
--header 'Authorization: Bearer {JWT token}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 81
Server: Crow/master
Date: Fri, 03 Nov 2023 04:07:06 GMT
Connection: Keep-Alive

Result: Company ID: 1; Company Name: CompanyA; Company email:1@gmail.com 
```

If authentication failed, response "invalid token".

### Re-apply for the access token:

#### Request
`POST /recoverCompany` 

```
curl -i --location 'http://localhost:3000/recoverCompany' \
--header 'Content-Type: text/plain' \
--data-raw '{
    "email" : "eg@gmail.com"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 40
Server: Crow/master
Date: Fri, 03 Nov 2023 04:12:38 GMT
Connection: Keep-Alive

An email has been send to your address 
```

Then, client needs to find his new API key in his email.

Failed (email has not been registered):
```
HTTP/1.1 400 Bad Request
Content-Length: 28
Server: Crow/master
Date: Fri, 03 Nov 2023 04:14:22 GMT
Connection: Keep-Alive

Your email has not been registered
```

### Update client's company info:
`PATCH /company/changeCompany` 
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/company/changeCompany' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--data '{
    "company_name": "newName"
}'
```
#### Response
```
HTTP/1.1 200 OK
Content-Length: 24
Server: Crow/master
Date: Fri, 01 Dec 2023 03:39:19 GMT
Connection: Keep-Alive

Update Company Success 
```

### Add a new member:
`POST /member/addMember`
#### Request

```
curl -i --location 'http://localhost:3000/member/addMember' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--data-raw '{
    "first_name" : "Jane",
    "last_name" : "Doe",
    "email": "jd@test.com",
    "password": "abc123",
    "phone_number": "111"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 20
Server: Crow/master
Date: Fri, 01 Dec 2023 03:47:38 GMT
Connection: Keep-Alive

Add Member Success 
```
Failed (duplicate member email):
```
HTTP/1.1 500 Internal Server Error
Content-Length: 82
Server: Crow/master
Date: Fri, 01 Dec 2023 03:48:10 GMT
Connection: Keep-Alive

Add Member Error: Duplicate entry 'jd@test.com-21' for key 'member_table.PRIMARY'
```

### Delete a member as admin:
`DELETE /admin/member/removeMember/<string:memberEmail>`
#### Request
```
curl -i --location --request DELETE 'http://localhost:3000/admin/member/removeMember/jd@test.com' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs'
```

#### Response

Success:
```
HTTP/1.1 204 No Content
Content-Length: 21
Server: Crow/master
Date: Fri, 01 Dec 2023 03:53:09 GMT
Connection: Keep-Alive
```

Failed (member not exist)：
```
HTTP/1.1 400 Bad Request
Content-Length: 25
Server: Crow/master
Date: Fri, 01 Dec 2023 03:53:51 GMT
Connection: Keep-Alive

No Matching Memeber Found
```

### Change member's info as admin:
`PATCH /admin/member/changeMemberInfo`
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/admin/member/changeMemberInfo' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--data-raw '{
    "first_name" : "newFN",
    "last_name" : "newLN",
    "email": "jd@test.com",
    "phone_number": "12345"
}'
```
#### Response
Success:
```
HTTP/1.1 200 OK
Content-Length: 14
Server: Crow/master
Date: Fri, 01 Dec 2023 04:03:02 GMT
Connection: Keep-Alive

Update success
```
Failed (Member email not exist):
```
HTTP/1.1 400 Bad Request
Content-Length: 24
Server: Crow/master
Date: Fri, 01 Dec 2023 04:02:26 GMT
Connection: Keep-Alive

No Query Found To Update
```

### Member login:
`POST /member/login`
#### Request
```
curl -i --location 'http://localhost:3000/member/login' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--data-raw '{
    "email": "jd@test.com",
    "password": "abc123"
}'
```

#### Response
Success:
```
HTTP/1.1 200 OK
Set-Cookie: PKdhtXMmr18n2L9K88eMlGn7CcctT9Rw=LJ8g; Path=/; Max-Age=86400
Content-Length: 18
Server: Crow/master
Date: Fri, 01 Dec 2023 04:06:47 GMT
Connection: Keep-Alive

Login Successfully
```

Failed (email and password not matching):
```
HTTP/1.1 401 Unauthorized
Content-Length: 18
Server: Crow/master
Date: Fri, 01 Dec 2023 04:11:05 GMT
Connection: Keep-Alive

401 Unauthorized
```

### Change member's info by self:
`PATCH /member/changeMemberInfo`
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/member/changeMemberInfo' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--header 'Cookie: PKdhtXMmr18n2L9K88eMlGn7CcctT9Rw=LJ8g' \
--data '{
    "first_name" : "FN",
    "last_name" : "LN",
    "phone_number": "12310"
}'
```
#### Response
Success (member needs to login and has correct cookies in header):
```
HTTP/1.1 200 OK
Content-Length: 14
Server: Crow/master
Date: Fri, 01 Dec 2023 04:09:20 GMT
Connection: Keep-Alive

Update success
```
Failed (member not logged in):
```
HTTP/1.1 400 Bad Request
Content-Length: 40
Server: Crow/master
Date: Fri, 01 Dec 2023 04:09:57 GMT
Connection: Keep-Alive

Auhorization Failed. Have not logged in.
```
### Member logout:
`POST /member/logout`

#### Request
```
curl -i --location --request POST 'http://localhost:3000/member/logout' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--header 'Cookie: PKdhtXMmr18n2L9K88eMlGn7CcctT9Rw=LJ8g'
```
#### Response
```
HTTP/1.1 200 OK
Content-Length: 19
Server: Crow/master
Date: Fri, 01 Dec 2023 04:12:39 GMT
Connection: Keep-Alive

Logout Successfully
```

### Get member's info by self:
`GET /member/profile`
#### Request
```
curl -i --location 'http://localhost:3000/member/profile' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--header 'Cookie: PKdhtXMmr18n2L9K88eMlGn7CcctT9Rw=LJ8g'
```
#### Response
Succcess：
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 78
Server: Crow/master
Date: Fri, 01 Dec 2023 04:15:02 GMT
Connection: Keep-Alive

{
   "email" : "jd@test.com",
   "first_name" : "FN",
   "last_name" : "LN"
}
```
Failed (member not logged in):
```
HTTP/1.1 400 Bad Request
Content-Length: 40
Server: Crow/master
Date: Fri, 01 Dec 2023 04:14:15 GMT
Connection: Keep-Alive

Auhorization Failed. Have not logged in.
```

### Add a new subscription:
`POST /subscription/addSubscription`
#### Request
```
curl -i --location 'http://localhost:3000/subscription/addSubscription' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--data-raw '{
    "member_email": "jd@test.com",
    "subscription_name": "general",
    "subscription_type": "yearly",
    "subscription_status": "activate",
    "next_due_date": "2023-12-30 00:00:00",
    "start_date": "2022-11-30 00:00:00",
    "billing_info": "info"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 26
Server: Crow/master
Date: Fri, 01 Dec 2023 04:18:41 GMT
Connection: Keep-Alive

Add Subscription Success
```
### Update subscription:
`PATCH /subscription/updateSubscription`

#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/subscription/updateSubscription' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzI1NzM1NjEsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIyMSJ9.gPSQ91ze5GL4CkCE_sRWyPoQRRcEuQsogoQEJ9cxeQs' \
--data-raw '{
    "email" : "jd@test.com",
    "subscription_name": "general",
    "new_action": "change subType"
}'
```
#### Response
Success:
```
HTTP/1.1 200 OK
Content-Length: 14
Server: Crow/master
Date: Fri, 01 Dec 2023 04:20:51 GMT
Connection: Keep-Alive

Update Success
```
Failed (no matching email and sbuscription name):
```
HTTP/1.1 400 Bad Request
Content-Length: 24
Server: Crow/master
Date: Fri, 01 Dec 2023 04:21:43 GMT
Connection: Keep-Alive

No Query Found To Update
```