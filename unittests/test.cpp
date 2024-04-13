
#include <gtest/gtest.h>
#include <enumstringifier.hpp>
#include <httpmessage.hpp>
#include <httpconstants.hpp>
#include <exception>
/*
#include <utpp/utpp.h>
#include <iostream>
#include <filesystem>
*/
//namespace fs = std::filesystem;

using namespace Http;

TEST (EnumStringifierTest, EnumToStringAssertion){
    EXPECT_STREQ("GET", Method::to_string(HttpMethod::GET).c_str());
}

TEST (EnumStringifierTest, EnumFromStringAssertion){
    EXPECT_EQ(HttpMethod::GET, Method::from_string("GET"));
}

TEST (EnumStringifierTest, EnumThrowAssertion){
   EXPECT_ANY_THROW(Method::from_string("NOT A VALID HTTP METHOD"));
}

TEST(ServerTest, RequestAssertion){
    HttpRequest  http_request;
    http_request.from_string("GET /index.html HTTP/1.1\r\nAccept: text/html\r\n\r\n");

    EXPECT_EQ(HttpMethod::GET, http_request.method());
    EXPECT_STREQ("/index.html", http_request.path().c_str());
    EXPECT_EQ(HttpVersion::Http_11, http_request.version());
    EXPECT_STREQ("text/html", http_request.header("Accept").c_str());
}

TEST(ServerTest, ResponseAssertion) {
    HttpResponse http_response;
    http_response.set_version(HttpVersion::Http_20);
    http_response.set_status_code(HttpStatusCode::Ok);
    const char* expected_response = "HTTP/2.0 200 Ok\r\n\r\n";

    EXPECT_STREQ(expected_response, http_response.to_string().c_str());
}
