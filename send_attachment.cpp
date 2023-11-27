#include <curl/curl.h>
#include "data_management.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

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

void send(const std::string &email, const std::string &attachment) {
    CURL *curl = curl_easy_init();

    if (curl) {
        const char *url = "https://api.sendgrid.com/v3/mail/send";
        struct curl_slist *recipients = NULL;
        struct curl_slist *headers = NULL;

        curl_easy_setopt(curl, CURLOPT_URL, url);

        //Put a private key here
        std::string auth = "";
        headers = curl_slist_append(headers, auth.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::ifstream pdfFile(attachment, std::ios::binary);
        std::ostringstream pdfStream;
        pdfStream << pdfFile.rdbuf();
        std::string pdfContent = pdfStream.str();
        std::string encoded = base64_encode(pdfContent);

        std::string jsonPayload = R"({
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
            "subject": "Picture",
            "content": [
                {
                    "type": "text/plain",
                    "value": "This is the picture"
                }
            ],
            "attachments": [
                {
                    "content": ")" + encoded + R"(",
                    "filename": "minimal.pdf",
                    "type": "application/pdf",
                    "disposition": "attachment"
                }
            ]

        })";


        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }
}
