/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "./data_management.hpp"

std::string base64_encode(const std::string &input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return result;
}

std::string make_payload(const std::string &email, const std::string &encoded, const std::string &message) {
    if (encoded.size() == 0) {
        return R"({
            "personalizations": [
                {
                    "to": [
                        {
                            "email": ")" + email + R"("
                        }
                    ]
                }
            ],
            "from": {
                "email": "hl3608@columbia.edu"
            },
            "subject": "Request Result",
            "content": [
                {
                    "type": "text/plain",
                    "value": ")" + message + R"("
                }
            ]
        })";
    }
    return R"({
            "personalizations": [
                {
                    "to": [
                        {
                            "email": ")" + email + R"("
                        }
                    ]
                }
            ],
            "from": {
                "email": "hl3608@columbia.edu"
            },
            "subject": "Request Result",
            "content": [
                {
                    "type": "text/plain",
                    "value": ")" + message + R"("
                }
            ],
            "attachments": [
                {
                    "content": ")" + encoded + R"(",
                    "filename": "figure.pdf",
                    "type": "application/pdf",
                    "disposition": "attachment"
                }
            ]

        })";
}

void send(const std::string &email, const std::string &attachment, const std::string &message) {
    CURL *curl = curl_easy_init();

    if (curl) {
        const char *url = "https://api.sendgrid.com/v3/mail/send";
        struct curl_slist *headers = NULL;

        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Put a private key here
        std::string auth = "";
        headers = curl_slist_append(headers, auth.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string encoded;
        if (attachment.size() > 0) {
            std::ifstream pdfFile(attachment, std::ios::binary);
            std::ostringstream pdfStream;
            pdfStream << pdfFile.rdbuf();
            std::string pdfContent = pdfStream.str();
            encoded = base64_encode(pdfContent);
        } else {
            encoded = "";
        }
        std::string jsonPayload = make_payload(email, encoded, message);


        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
}
