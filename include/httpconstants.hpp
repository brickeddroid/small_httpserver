#ifndef HTTPCONSTANTS_HPP
#define HTTPCONSTANTS_HPP

//#include <cstring>
//#include <algorithm>
#include <string>
#include <map>


enum class HttpVersion {
    Http_09 = 9,
    Http_10 = 10,
    Http_11 = 11,
    Http_20 = 20,
    Http_30 = 30,
    UNKNOWN
};

enum class HttpStatusCode {
    Continue = 100,
    SwitchingProtocols = 101,
    EarlyHints = 103,
    Ok = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    RequestTimeout = 408,
    ImATeapot = 418,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnvailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505
};

enum class HttpMethod {
    HEAD,
    GET,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
    UNKNOWN
};

std::string version_to_string(HttpVersion version);
std::string status_to_string(HttpStatusCode status);
std::string method_to_string(HttpMethod method);

HttpVersion version_from_string(const std::string& ver_str);
HttpStatusCode status_from_string(const std::string& sta_str);
HttpMethod method_from_string(const std::string& met_str);

#endif
