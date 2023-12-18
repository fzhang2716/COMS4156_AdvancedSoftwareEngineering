## REST API Curl Example Demo

For the following request, if the client did not provide correct request syntax, for example, missing parameters, then the server responses "400 Bad Request" like this:

```
HTTP/1.1 400 Bad Request
Content-Length: 35
Server: Crow/master
Date: Mon, 18 Dec 2023 05:06:07 GMT
Connection: Keep-Alive

{
   "error" : "Invalid request"
}
```

If the client did not provide JWT token for certain requests, the server responses "401 Unauthorized" like this:
```
HTTP/1.1 401 Unauthorized
Content-Length: 21
Server: Crow/master
Date: Mon, 18 Dec 2023 05:06:43 GMT
Connection: Keep-Alive

JWT token not found 
``` 

If the client provided invalid JWT token for certain requests, the server responses "401 Unauthorized" like this:
```
HTTP/1.1 401 Unauthorized
Content-Length: 15
Server: Crow/master
Date: Mon, 18 Dec 2023 05:07:33 GMT
Connection: Keep-Alive

Invalid token 
```


### Register as a new client (company):

#### Request
`POST company/addCompany`

```
curl -i --location 'http://localhost:3000/company/addCompany' \
--header 'Content-Type: text/plain' \
--data-raw '{
    "email" : "eg2@gmail.com",
    "company_name" : "EG"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 204
Server: Crow/master
Date: Mon, 18 Dec 2023 05:08:44 GMT
Connection: Keep-Alive

{
   "msg" : "Add Company Success",
   "tokenMsg" : "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MzQ0MTIxMjQsImlzcyI6IlN1Yk1hbmFnZXIiLCJzdWIiOiIxODQifQ.1unTMh9CPyOLPC_eqQdDlHl7iCOPD3gjFk55jYj1w0o"
}
```

Failed (company email has been registered):

e.g. duplicate id
```
HTTP/1.1 500 Internal Server Error
Content-Length: 139
Server: Crow/master
Date: Mon, 18 Dec 2023 05:08:17 GMT
Connection: Keep-Alive

{
   "error" : "Add Company Error: You have already registered with this email, if you lost your JWT token, please apply for a new one."
}
```

### Get company's info:

#### Request
`GET /company` 

```
curl -i --location 'http://localhost:3000/company' \
--header 'Authorization: Bearer {JWT TOKEN}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 103
Server: Crow/master
Date: Mon, 18 Dec 2023 05:09:31 GMT
Connection: Keep-Alive

{
   "company_id" : "21",
   "company_name" : "AliceOutlook",
   "email" : "liuhanalice@outlook.com"
}
```

If authentication failed, response "invalid token".

### Company re-apply for the access token:

#### Request
`POST /recoverCompany` 

```
curl -i --location 'http://localhost:3000/recoverCompany' \
--header 'Content-Type: text/plain' \
--data-raw '{
    "email" : "eg2@gmail.com"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 56
Server: Crow/master
Date: Mon, 18 Dec 2023 05:10:51 GMT
Connection: Keep-Alive

{
   "msg" : "An email has been send to your address"
}
```

Then, client needs to find his new API key in his email.

Failed (email has not been registered):
```
HTTP/1.1 400 Bad Request
Content-Length: 54
Server: Crow/master
Date: Mon, 18 Dec 2023 05:11:20 GMT
Connection: Keep-Alive

{
   "error" : "Your email has not been registered"
}
```

### Update company's info:
`PATCH /company/changeCompany` 
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/company/changeCompany' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data '{
    "company_name": "Name"
}'
```
#### Response
```
HTTP/1.1 200 OK
Content-Length: 40
Server: Crow/master
Date: Mon, 18 Dec 2023 05:13:05 GMT
Connection: Keep-Alive

{
   "msg" : "Update Company Success"
}
```

### Add a new member:
`POST /member/addMember`
#### Request

```
curl -i --location 'http://localhost:3000/member/addMember' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data-raw '{
     "first_name" : "Test",
     "last_name" : "Name",
     "email": "tn@test.com",
     "password": "123",
     "phone_number": "1234567"
}
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 36
Server: Crow/master
Date: Mon, 18 Dec 2023 05:14:44 GMT
Connection: Keep-Alive

{
   "msg" : "Add Member Success"
}
```

Failed (duplicate member email):
```
HTTP/1.1 500 Internal Server Error
Content-Length: 101
Server: Crow/master
Date: Mon, 18 Dec 2023 05:15:43 GMT
Connection: Keep-Alive

{
   "error" : "Add Member Error: Duplicate entry 'tn@test.com-21' for key 'member_table.PRIMARY'"
}
```

### Get company's all members:
`GET /company/getMembers?page=<int>&pagesize=<int>`
#### Request
```
curl -i --location 'http://localhost:3000/company/getMembers?page=1&pagesize=10' \
--header 'Authorization: Bearer {JWT TOKEN}' \
```

#### Response
Success:
```
{
   "members" : [
      {
         "email" : "1111@test.com",
         "first_name" : "Amy",
         "last_name" : "Wang",
         "phone_number" : "12345678343"
      },
      {
         "email" : "1223@test.com",
         "first_name" : "Nick",
         "last_name" : "Lee",
         "phone_number" : "12335689"
      },
      {
         "email" : "123@test.com",
         "first_name" : "Aa",
         "last_name" : "Bb",
         "phone_number" : "123456"
      },
      {
         "email" : "1234@test.com",
         "first_name" : "Frontend",
         "last_name" : "F",
         "phone_number" : "123456"
      },
      {
         "email" : "214@test.com",
         "first_name" : "Frontend11",
         "last_name" : "F",
         "phone_number" : "123456"
      },
      {
         "email" : "33@test.com",
         "first_name" : "Dave",
         "last_name" : "Smith",
         "phone_number" : "123"
      },
      {
         "email" : "44@test.com",
         "first_name" : "Amy",
         "last_name" : "Wang",
         "phone_number" : "123"
      },
      {
         "email" : "55@test.com",
         "first_name" : "Bob",
         "last_name" : "Grant",
         "phone_number" : "123"
      },
      {
         "email" : "61@test.com",
         "first_name" : "Tina",
         "last_name" : "Wang",
         "phone_number" : "123"
      },
      {
         "email" : "68@test.com",
         "first_name" : "Lena",
         "last_name" : "Musk",
         "phone_number" : "123"
      }
   ],
   "total_members" : "17",
   "total_pages" : "2"
}
```


### Delete a member as admin:
`DELETE /admin/member/removeMember/<string:memberEmail>`
#### Request
```
curl -i --location --request DELETE 'http://localhost:3000/admin/member/removeMember/tn@test.com' \
--header 'Authorization: Bearer {JWT TOKEN}' \
```

#### Response

Success:
```
HTTP/1.1 204 No Content
Content-Length: 39
Server: Crow/master
Date: Mon, 18 Dec 2023 05:21:22 GMT
Connection: Keep-Alive
```

Failed (member not exist)：
```
HTTP/1.1 400 Bad Request
Content-Length: 45
Server: Crow/master
Date: Mon, 18 Dec 2023 05:21:35 GMT
Connection: Keep-Alive

{
   "error" : "No Matching Memeber Found"
}
```

### Change member's info as admin:
`PATCH /admin/member/changeMemberInfo`
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/admin/member/changeMemberInfo' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
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
Content-Length: 32
Server: Crow/master
Date: Mon, 18 Dec 2023 05:22:40 GMT
Connection: Keep-Alive

{
   "msg" : "Update success"
}
```
Failed (Member email not exist):
```
HTTP/1.1 400 Bad Request
Content-Length: 24
Server: Crow/master
Date: Mon, 18 Dec 2023 05:22:55 GMT
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
Set-Cookie: PKdhtXMmr18n2L9K88eMlGn7CcctT9Rw=WzC2; Path=/; Max-Age=86400
Content-Length: 43
Server: Crow/master
Date: Mon, 18 Dec 2023 05:24:07 GMT
Connection: Keep-Alive

{
   "msg" : "success"
}
Login Successfully
```

Failed (email and password not matching):
```
HTTP/1.1 401 Unauthorized
Content-Length: 32
Server: Crow/master
Date: Mon, 18 Dec 2023 05:24:26 GMT
Connection: Keep-Alive

{
   "error" : "Unauthorized"
}
```

### Change member's info by self:
`PATCH /member/changeMemberInfo`
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/member/changeMemberInfo' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--header 'Cookie: {LOGIN COOKIES}' \
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
Content-Length: 32
Server: Crow/master
Date: Mon, 18 Dec 2023 05:25:38 GMT
Connection: Keep-Alive

{
   "msg" : "Update success"
}
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
--header 'Authorization: Bearer {JWT TOKEN}' \
--header 'Cookie: {LOGIN COOKIES}'
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
--header 'Authorization: Bearer {JWT TOKEN}' \
--header 'Cookie: {LOGIN COOKIES}'
```
#### Response
Succcess：
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 113
Server: Crow/master
Date: Mon, 18 Dec 2023 05:28:43 GMT
Connection: Keep-Alive

{
   "email" : "jd@test.com",
   "first_name" : "newFN",
   "last_name" : "newLN",
   "phone_number" : "12345"
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

### Get member's info as admin:
`GET /admin/member/profile/<string:member_email>`

#### Request
```
curl -i --location 'http://localhost:3000/admin/member/profile/jd@test.com' \
--header 'Authorization: Bearer {JWT TOKEN}' \
```

#### Response
Success:
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 113
Server: Crow/master
Date: Mon, 18 Dec 2023 05:30:42 GMT
Connection: Keep-Alive

{
   "email" : "jd@test.com",
   "first_name" : "newFN",
   "last_name" : "newLN",
   "phone_number" : "12345"
}
```
Failed (No matching member):

```
HTTP/1.1 400 Bad Request
Content-Type: application/json
Content-Length: 71
Server: Crow/master
Date: Mon, 18 Dec 2023 05:31:30 GMT
Connection: Keep-Alive

{
   "error" : "No member found for the specified company and email"
}
```

### Add a new subscription:
`POST /subscription/addSubscription`
#### Request
```
curl -i --location 'http://localhost:3000/subscription/addSubscription' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer{JWT TOKEN}' \
--data-raw '{
    "member_email": "jd@test.com",
    "subscription_name": "curl",
    "subscription_type": "Pro",
    "subscription_status": "activate",
    "next_due_date": "2023-12-17 00:00:00",
    "start_date": "2024-01-17 00:00:00",
    "billing_info": "0123 credit card"
}'
```

#### Response

Success:
```
HTTP/1.1 200 OK
Content-Length: 42
Server: Crow/master
Date: Mon, 18 Dec 2023 05:32:49 GMT
Connection: Keep-Alive

{
   "msg" : "Add Subscription Success"
}
```

Failed:
```
HTTP/1.1 400 Bad Request
Content-Length: 37
Server: Crow/master
Date: Mon, 18 Dec 2023 05:33:42 GMT
Connection: Keep-Alive

{
   "error" : "Member Not Exists"
}
```
### Update subscription as admin:
`PATCH /admin/subscription/updateSubscription`

#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/admin/subscription/updateSubscription' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data '{
    "subscription_id" : "13",
    "subscription_name": "newName",
    "subscription_type": "Pro",
    "subscription_status" : "renewed",
    "start_date" : "2024-01-01 23:02:00",
    "next_due_date": "2024-02-01 23:02:00",
    "billing_info" : "credit card"
}'
```
#### Response
Success:
```
HTTP/1.1 200 OK
Content-Length: 32
Server: Crow/master
Date: Mon, 18 Dec 2023 05:35:12 GMT
Connection: Keep-Alive

{
   "msg" : "Update Success"
}
```
Failed (no matching subscription id):
```
HTTP/1.1 400 Bad Request
Content-Length: 99
Server: Crow/master
Date: Mon, 18 Dec 2023 05:35:32 GMT
Connection: Keep-Alive

{
   "error" : "No subscription found or you don't have permission to modify this subscription."
}
```

### Update subscription by self:
`PATCH /subscription/updateSubscription`
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/subscription/updateSubscription' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--header 'Cookie: {LOGIN COOKIES}' \
--data '{
    "subscription_id" : "11",
    "subscription_status": "activated",
    "billing_info": "postman test"
}'
```

#### Response
Success:
```
HTTP/1.1 200 OK
Content-Length: 45
Server: Crow/master
Date: Mon, 18 Dec 2023 05:37:45 GMT
Connection: Keep-Alive

{
   "msg" : "Update Subscription Success"
}
```

Failed:
```
HTTP/1.1 400 Bad Request
Content-Length: 40
Server: Crow/master
Date: Mon, 18 Dec 2023 05:38:18 GMT
Connection: Keep-Alive

Auhorization Failed. Have not logged in.
```

### Update subscription modify action:
`PATCH /subscription/updateSubscriptionAction`
#### Request
```
curl -i --location --request PATCH 'http://localhost:3000/subscription/updateSubscriptionAction' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data '{
    "subscription_id": "11",
    "last_action": "canceled subscription"
}'
```
#### Response
```
HTTP/1.1 200 OK
Content-Length: 52
Server: Crow/master
Date: Mon, 18 Dec 2023 05:40:53 GMT
Connection: Keep-Alive

{
   "msg" : "Update Subscription Action Success"
}
```

### View a member's subscription as admin:
`GET /admin/subscription/viewSubscriptions`

#### Request
```
curl -i --location --request GET 'http://localhost:3000/admin/subscription/viewSubscriptions' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data-raw '{
    "email" : "jd@test.com"
}'
```

#### Response
Success:
```
{
   "subscriptions" : [
      {
         "billing_info" : "postman test",
         "last_action" : "canceled subscription",
         "last_action_date" : "2023-12-18 00:40:53",
         "next_due_date" : "2023-12-30 00:00:00",
         "start_date" : "2022-11-30 00:00:00",
         "subscription_id" : "11",
         "subscription_name" : "general",
         "subscription_status" : "activated",
         "subscription_type" : "yearly"
      },
      {
         "billing_info" : "0123 credit card",
         "last_action" : "",
         "last_action_date" : "",
         "next_due_date" : "2023-12-17 00:00:00",
         "start_date" : "2024-01-17 00:00:00",
         "subscription_id" : "170",
         "subscription_name" : "curl",
         "subscription_status" : "activate",
         "subscription_type" : "Pro"
      }
   ],
   "total_subscriptions" : "2"
}
```
### View a member's subscription by self:
`GET /subscription/viewSubscriptions`
#### Request
```
curl -i --location 'http://localhost:3000/subscription/viewSubscriptions' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--header 'Cookie: {LOGIN COOKIES}'
```

#### Response
Success:
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 699
Server: Crow/master
Date: Mon, 18 Dec 2023 05:44:51 GMT
Connection: Keep-Alive

{
   "subscriptions" : [
      {
         "billing_info" : "postman test",
         "next_due_date" : "2023-12-30 00:00:00",
         "start_date" : "2022-11-30 00:00:00",
         "subscription_id" : "11",
         "subscription_name" : "general",
         "subscription_status" : "activated",
         "subscription_type" : "yearly"
      },
      {
         "billing_info" : "0123 credit card",
         "next_due_date" : "2023-12-17 00:00:00",
         "start_date" : "2024-01-17 00:00:00",
         "subscription_id" : "170",
         "subscription_name" : "curl",
         "subscription_status" : "activate",
         "subscription_type" : "Pro"
      }
   ],
   "total_subscriptions" : "2"
}
```
Failed:
```
HTTP/1.1 400 Bad Request
Content-Length: 40
Server: Crow/master
Date: Mon, 18 Dec 2023 05:45:33 GMT
Connection: Keep-Alive

Auhorization Failed. Have not logged in.
```

### Get all subscriptions of a company:
`GET /subscription/allSubscriptions?page=<int>&pagesize=<int>`
#### Request
```
curl -i --location 'http://localhost:3000/subscription/allSubscriptions?page=1&pagesize=5' \
--header 'Authorization: Bearer {JWT TOKEN}' \
```

#### Response
Success:
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 2199
Server: Crow/master
Date: Mon, 18 Dec 2023 05:47:41 GMT
Connection: Keep-Alive

{
   "subscriptions" : [
      {
         "billing_info" : "6548 credit card",
         "last_action" : "",
         "last_action_date" : "",
         "member_email" : "1111@test.com",
         "next_due_date" : "2024-01-06 20:47:00",
         "start_date" : "2023-12-06 20:46:00",
         "subscription_id" : "115",
         "subscription_name" : "TV",
         "subscription_status" : "activated",
         "subscription_type" : "plus"
      },
      {
         "billing_info" : "0991 credit card",
         "last_action" : "",
         "last_action_date" : "",
         "member_email" : "1223@test.com",
         "next_due_date" : "2024-01-05 18:23:00",
         "start_date" : "2023-12-05 18:23:00",
         "subscription_id" : "38",
         "subscription_name" : "TV",
         "subscription_status" : "activated",
         "subscription_type" : "pro"
      },
      {
         "billing_info" : "8214 credit card",
         "last_action" : "",
         "last_action_date" : "",
         "member_email" : "55@test.com",
         "next_due_date" : "2024-01-06 20:47:00",
         "start_date" : "2023-12-06 20:47:00",
         "subscription_id" : "116",
         "subscription_name" : "TV",
         "subscription_status" : "activated",
         "subscription_type" : "general"
      },
      {
         "billing_info" : "NA",
         "last_action" : "",
         "last_action_date" : "",
         "member_email" : "55@test.com",
         "next_due_date" : "2024-01-06 20:48:00",
         "start_date" : "2023-12-06 20:48:00",
         "subscription_id" : "117",
         "subscription_name" : "Internet",
         "subscription_status" : "activated",
         "subscription_type" : "free"
      },
      {
         "billing_info" : "0000 credit card",
         "last_action" : "",
         "last_action_date" : "",
         "member_email" : "egg@test.com",
         "next_due_date" : "2024-01-17 20:08:00",
         "start_date" : "2023-12-17 20:08:00",
         "subscription_id" : "152",
         "subscription_name" : "General",
         "subscription_status" : "activated",
         "subscription_type" : "general"
      }
   ],
   "total_pages" : "4",
   "total_subscriptions" : "19"
}
```

### Get expiring subscriptions of a company:
`GET /company/getExpiringSubscriptionByTime?subscription_name=<string>V&days=<int>`

#### Request
```
curl -i --location 'http://localhost:3000/company/getExpiringSubscriptionByTime?subscription_name=TV&days=100' \
--header 'Authorization: Bearer {JWT TOKEN}' \
```

#### Response
```
HTTP/1.1 200 OK
Content-Length: 140
Server: Crow/master
Date: Mon, 18 Dec 2023 05:49:15 GMT
Connection: Keep-Alive

{
   "0" : "1223@test.com",
   "1" : "1111@test.com",
   "2" : "55@test.com",
   "number" : "3",
   "target_time" : "2024-03-27 01:49:15"
}
```

### Send email reminders to members:
`POST /company/sendReminder`
#### Request
```
curl -i --location 'http://localhost:3000/company/sendReminder' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data-raw '{
   "0" : "1223@test.com",
   "1" : "1111@test.com",
   "2" : "55@test.com",
   "number" : "3",
   "target_time" : "2024-03-27 01:49:15"
}
'
```
#### Response
Success:
```
HTTP/1.1 200 OK
Content-Length: 35
Server: Crow/master
Date: Mon, 18 Dec 2023 05:51:07 GMT
Connection: Keep-Alive

{
   "msg" : "Send successfully"
}
```


### Send company analysis report of subscription duration:
`POST /company/analyzeSubDuration`
#### Request
```
curl -i --location 'http://localhost:3000/company/analyzeSubDuration' \
--header 'Content-Type: application/json' \
--header 'Authorization: Bearer {JWT TOKEN}' \
--data-raw '{
    "email" : "company@test.com"
}'
```

#### Response
Success:
```
HTTP/1.1 200 OK
Content-Type: application/json
Content-Length: 38
Server: Crow/master
Date: Mon, 18 Dec 2023 05:52:54 GMT
Connection: Keep-Alive

{
   "msg" : "Analyze successfully"
}
```