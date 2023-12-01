# COMS4156_AdvancedSoftwareEngineering

## Overview
Our project is a subscription management service that provides sub-services like subscription data management, analytics, insights feedback, notification sending, alerting, and reports generation.

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
```

## Style Check
We use Cpplint, which is a command-line tool to check C/C++ files for style issues following Google's C++ style guide.
```
pip install cpplint
cpplint [filename]
```

## REST API

Refer to this [document](https://docs.google.com/document/d/1liqiSBghhSN7_ZPRPIBlw0qJ3BbGde0nJYx_kBdFqIo/edit?usp=sharing) (Lion Mail Required) to see example API request using Postman.

For the following request, if the client did not provide correct request syntax, for example, missing parameters, then the server responses "400 Bad Request" like this:

```
HTTP/1.1 400 Bad Request
Content-Length: 17
Server: Crow/master
Date: Thu, 19 Oct 2023 21:39:58 GMT
Connection: Keep-Alive

Invalid request 
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

If succeed:
```
HTTP/1.1 200 OK
Content-Length: 195
Server: Crow/master
Date: Fri, 03 Nov 2023 04:10:27 GMT
Connection: Keep-Alive

Add Company Success 
Please save your JWT token: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzA1MjA2MjgsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIxNiJ9.kfC9TVEi8AgCLSrXF8obpQLi76gjXhdvtP5M6_kJaXU
```

If failed:

e.g. duplicate id
```
HTTP/1.1 500 Internal Server Error
Content-Length: 119
Server: Crow/master
Date: Fri, 03 Nov 2023 04:10:45 GMT
Connection: Keep-Alive

Add Company Error: You have already registered with this email, if you lost your JWT token, please apply for a new one.
```

### Get user's company info:

#### Request
`GET /company` 

```
curl -i --location 'http://localhost:3000/company' \
--header 'Authorization: Bearer {JWT token}'
```

#### Response

If authentication succeed (client provided matching username and password):
```
HTTP/1.1 200 OK
Content-Length: 81
Server: Crow/master
Date: Fri, 03 Nov 2023 04:07:06 GMT
Connection: Keep-Alive

Result: Company ID: 1; Company Name: CompanyA; Company email:1@gmail.com 
```

If authentication failed (client did not provide matching username and password):
```
HTTP/1.1 401 Unauthorized
Content-Length: 15
Server: Crow/master
Date: Fri, 03 Nov 2023 04:08:01 GMT
Connection: Keep-Alive

Invalid token 
```
### Re-apply for the access token:

#### Request
`GET /recoverCompany` 

```
curl -i --location 'http://localhost:3000/recoverCompany' \
--header 'Content-Type: text/plain' \
--data-raw '{
    "email" : "eg@gmail.com"
}'
```

#### Response

If authentication succeed (client provided matching username and password):
```
HTTP/1.1 200 OK
Content-Length: 40
Server: Crow/master
Date: Fri, 03 Nov 2023 04:12:38 GMT
Connection: Keep-Alive

An email has been send to your address 
```

Then, client needs to find his new API key in his email.

If authentication failed (email has not been registered):
```
HTTP/1.1 400 Bad Request
Content-Length: 28
Server: Crow/master
Date: Fri, 03 Nov 2023 04:14:22 GMT
Connection: Keep-Alive

Your email has not been registered
```

## TODO: update below
### Add a new member:

#### Request

`POST /addMember`
```
curl -i -L -X POST 'http://localhost:3000/addMember' -H 'Content-Type: text/plain' --data-raw '{
    "member_id": "8",
    "first_name": "jane",
    "last_name": "he",
    "email": "jh@mail.com",
    "phone_number": "12345",
    "member_status": "silver"
}'
```

#### Response

If succeed:
```
HTTP/1.1 200 OK
Content-Length: 20
Server: Crow/master
Date: Thu, 19 Oct 2023 21:46:17 GMT
Connection: Keep-Alive

Add Member Success 
```
If failed:

e.g. duplicate member id:
```
HTTP/1.1 500 Internal Server Error
Content-Length: 69
Server: Crow/master
Date: Thu, 19 Oct 2023 21:49:50 GMT
Connection: Keep-Alive

Add Member Error: Duplicate entry '8' for key 'member_table.PRIMARY'
```


### Add a new subscription:

#### Request

`POST /addSubscription`
```
curl -i -L -X POST 'http://localhost:3000/addSubscription' \
-H 'Content-Type: text/plain' \
--data '{
    "subscription_id" : "10",
    "member_id": "4",
    "company_id": "1",
    "subscription_type" : "Free",
    "subscription_status" : "active",
    "next_due_date" : "2023-10-29 00:00:00",
    "start_date" : "2023-9-29 00:00:00",
    "billing_info": "sth"
}'
```

#### Response

If succeed:
```
HTTP/1.1 200 OK
Content-Length: 26
Server: Crow/master
Date: Thu, 19 Oct 2023 21:53:10 GMT
Connection: Keep-Alive

Add Subscription Success 
```
If failed:

e.g. duplicate subscription id:
```
HTTP/1.1 500 Internal Server Error
Content-Length: 82
Server: Crow/master
Date: Thu, 19 Oct 2023 21:53:28 GMT
Connection: Keep-Alive

Add Subscription Error: Duplicate entry '10' for key 'subscription_table.PRIMARY'
```
