# COMS4156_AdvancedSoftwareEngineering

## Overview
Our project is a subscription management service that provides sub-services like subscription data management, analytics, insights feedback, notification sending, alerting, and reports generation.

## Install (Linux)
### [Crow](https://crowcpp.org/master/getting_started/setup/linux/)
( C++ framework for creating HTTP or Websocket web services )

1. ```
    git clone https://github.com/CrowCpp/Crow.git
    ```
2. navigate to Crow, then
    ```
    mkdir build
    ```
3. navigate to build
4. ```
    sudo apt install libasio-dev
    ```
5. ```
    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF
    ```
6. ```
    make install
    ```

### [MySQL Connector/C++](https://dev.mysql.com/doc/dev/connector-cpp/latest/)
( C++ library for applications to communicate with MySQL database servers )

1. ```
    sudo apt update
    ```
2. ```
    sudo apt-get install libmysqlcppconn-dev
    ```


### [Catch2](https://github.com/catchorg/Catch2)
( A unit testing framework for C++)
1. ```
    git clone -b v2.x https://github.com/catchorg/Catch2.git
    ```
2. ```
    cd Catch2
    ```
3. ```
    cmake -Bbuild -H. -DBUILD_TESTING=OFF
    ```
4. ```
    sudo cmake --build build/ --target install
    ```

## Compile and Run
In root directory, run Makefile:

```
make
```

## REST API

### Get user's company info

#### Request
`GET /company?username=usr&password=pwd` 

```
curl -G -i -d "username=company1" -d "password=pwd" http://localhost:3000/company
```

#### Response

If authentication success:
```
HTTP/1.1 200 OK
Content-Length: 106
Server: Crow/master
Date: Wed, 18 Oct 2023 23:42:34 GMT
Connection: Keep-Alive

Authentication success 
Result: Company ID: 1; Company Name: company1; Company email: company1@gmail.com
```

If authentication failed:
```
HTTP/1.1 401 Unauthorized
Content-Length: 23
Server: Crow/master
Date: Wed, 18 Oct 2023 23:43:37 GMT
Connection: Keep-Alive

Authentication failed 
```

### Create a new user (company)

#### Request
`POST /addCompany?company_id=id&email=email&hash_pwd=pwd&company_name=name`

```
curl -i -L -X POST 'http://localhost:3000/addCompany?company_id=60&email=com60@gmail.com&hash_pwd=12321&company_name=com60'
```

#### Response

If success:
```
HTTP/1.1 200 OK
Content-Length: 21
Server: Crow/master
Date: Thu, 19 Oct 2023 19:26:19 GMT
Connection: Keep-Alive

Add Company Success 
```

If failed:
```
HTTP/1.1 500 Internal Server Error
Content-Length: 27
Server: Crow/master
Date: Thu, 19 Oct 2023 19:29:53 GMT
Connection: Keep-Alive

500 Internal Server Error
```