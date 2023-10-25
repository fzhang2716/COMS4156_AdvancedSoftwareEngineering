# COMS4156_AdvancedSoftwareEngineering

Install crow:
1. "git clone https://github.com/CrowCpp/Crow.git"
2. navigate to Crow && "mkdir build"
3. navigate to build
4. "sudo apt install libasio-dev"
5. "cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF"
6. make install

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
./main
```

## Test
In root directory, run Makefile:
```
make test
./test
```

Result:
```
===============================================================================
All tests passed (4 assertions in 4 test cases)
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

### Get user's company info:

#### Request
`GET /company?username=usr&password=pwd` 

```
curl -G -i -d "username=company1" -d "password=pwd" http://localhost:3000/company
```

#### Response

If authentication succeed (client provided matching username and password):
```
HTTP/1.1 200 OK
Content-Length: 106
Server: Crow/master
Date: Wed, 18 Oct 2023 23:42:34 GMT
Connection: Keep-Alive

Authentication success 
Result: Company ID: 1; Company Name: company1; Company email: company1@gmail.com
```

If authentication failed (client did not provide matching username and password):
```
HTTP/1.1 401 Unauthorized
Content-Length: 23
Server: Crow/master
Date: Wed, 18 Oct 2023 23:43:37 GMT
Connection: Keep-Alive

Authentication failed 
```

### Create a new user (company):

#### Request
`POST /addCompany?company_id=id&email=email&hash_pwd=pwd&company_name=name`

```
curl -i -L -X POST 'http://localhost:3000/addCompany?company_id=60&email=com60@gmail.com&hash_pwd=12321&company_name=com60'
```

#### Response

If succeed:
```
HTTP/1.1 200 OK
Content-Length: 21
Server: Crow/master
Date: Thu, 19 Oct 2023 19:26:19 GMT
Connection: Keep-Alive

Add Company Success 
```

If failed:

e.g. duplicate id
```
HTTP/1.1 500 Internal Server Error
Content-Length: 72
Server: Crow/master
Date: Thu, 19 Oct 2023 21:39:02 GMT
Connection: Keep-Alive

Add Company Error: Duplicate entry '60' for key 'company_table.PRIMARY'
```

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
