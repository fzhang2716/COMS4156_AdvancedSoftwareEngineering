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


### [Fakeit](https://github.com/eranpeer/FakeIt)
1. ```
    git clone https://github.com/Microsoft/vcpkg.git
    ```
2. ```
    cd vcpkg
    ```
3. ```
    ./bootstrap-vcpkg.sh
    ```
4. ```
    ./vcpkg integrate install
    ```
5. ```
    ./vcpkg install fakeit
    ```
(Add '-I"vcpkg/packages/fakeit_x64-linux/include/standalone"' when compiling)

## Compile
Run Makefile:

```
make
```

## REST API

### Get user's company info

#### Request
`GET \company?company?companyname=usr&password=pwd` 

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

