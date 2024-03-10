#ifndef HTTPCONSTANTS_HPP
#define HTTPCONSTANTS_HPP

#include <enumstringifier.hpp>

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
    ANY
};

namespace Http {
const std::map<HttpVersion, std::string> version_map_to_string = {
    { HttpVersion::Http_09, "HTTP/0.9" },
    { HttpVersion::Http_10, "HTTP/1.0" },
    { HttpVersion::Http_11, "HTTP/1.1" },
    { HttpVersion::Http_20, "HTTP/2.0" },
    { HttpVersion::Http_30, "HTTP/3.0" }
};

const std::map<std::string, HttpVersion> version_map_from_string = {
    { "HTTP/0.9", HttpVersion::Http_09 },
    { "HTTP/1.0", HttpVersion::Http_10 },
    { "HTTP/1",   HttpVersion::Http_20 },
    { "HTTP/1.1", HttpVersion::Http_11 },
    { "HTTP/2.0", HttpVersion::Http_20 },
    { "HTTP/2",   HttpVersion::Http_20 },
    { "HTTP/3.0", HttpVersion::Http_30 }
};

const std::map<HttpStatusCode, std::string> status_map_to_string = {
    { HttpStatusCode::Continue, "Continue"},
    { HttpStatusCode::SwitchingProtocols, "SwitchingProtocols"},
    { HttpStatusCode::EarlyHints, "EarlyHints"},
    { HttpStatusCode::Ok, "Ok"},
    { HttpStatusCode::Created, "Created"},
    { HttpStatusCode::Accepted, "Accepted"},
    { HttpStatusCode::NonAuthoritativeInformation, "NonAuthoritativeInformation"},
    { HttpStatusCode::NoContent, "NoContent"},
    { HttpStatusCode::ResetContent, "ResetContent"},
    { HttpStatusCode::PartialContent, "PartialContent"},
    { HttpStatusCode::MultipleChoices, "MultipleChoices"},
    { HttpStatusCode::MovedPermanently, "MovedPermanently"},
    { HttpStatusCode::Found, "Found"},
    { HttpStatusCode::NotModified, "NotModified"},
    { HttpStatusCode::BadRequest, "BadRequest"},
    { HttpStatusCode::Unauthorized, "Unauthorized"},
    { HttpStatusCode::Forbidden, "Forbidden"},
    { HttpStatusCode::NotFound, "NotFound"},
    { HttpStatusCode::MethodNotAllowed, "MethodNotAllowed"},
    { HttpStatusCode::RequestTimeout, "RequestTimeout"},
    { HttpStatusCode::ImATeapot, "ImATeapot"},
    { HttpStatusCode::InternalServerError, "InternalServerError"},
    { HttpStatusCode::NotImplemented, "NotImplemented"},
    { HttpStatusCode::BadGateway, "BadGateway"},
    { HttpStatusCode::ServiceUnvailable, "ServiceUnvailable"},
    { HttpStatusCode::GatewayTimeout, "GatewayTimeout"},
    { HttpStatusCode::HttpVersionNotSupported, "HttpVersionNotSupported"}
};

const std::map<std::string, HttpStatusCode> status_map_from_string = {
    {"Continue", HttpStatusCode::Continue},
    {"SwitchingProtocols", HttpStatusCode::SwitchingProtocols},
    {"EarlyHints", HttpStatusCode::EarlyHints},
    {"Ok", HttpStatusCode::Ok},
    {"Created", HttpStatusCode::Created},
    {"Accepted", HttpStatusCode::Accepted},
    {"NonAuthoritativeInformation", HttpStatusCode::NonAuthoritativeInformation},
    {"NoContent", HttpStatusCode::NoContent},
    {"ResetContent", HttpStatusCode::ResetContent},
    {"PartialContent", HttpStatusCode::PartialContent},
    {"MultipleChoices", HttpStatusCode::MultipleChoices},
    {"MovedPermanently", HttpStatusCode::MovedPermanently},
    {"Found", HttpStatusCode::Found},
    {"NotModified", HttpStatusCode::NotModified},
    {"BadRequest", HttpStatusCode::BadRequest},
    {"Unauthorized", HttpStatusCode::Unauthorized},
    {"Forbidden", HttpStatusCode::Forbidden},
    {"NotFound", HttpStatusCode::NotFound},
    {"MethodNotAllowed", HttpStatusCode::MethodNotAllowed},
    {"RequestTimeout", HttpStatusCode::RequestTimeout},
    {"ImATeapot", HttpStatusCode::ImATeapot},
    {"InternalServerError", HttpStatusCode::InternalServerError},
    {"NotImplemented", HttpStatusCode::NotImplemented},
    {"BadGateway", HttpStatusCode::BadGateway},
    {"ServiceUnvailable", HttpStatusCode::ServiceUnvailable},
    {"GatewayTimeout", HttpStatusCode::GatewayTimeout},
    {"HttpVersionNotSupported", HttpStatusCode::HttpVersionNotSupported}
};

const std::map<HttpMethod, std::string> method_map_to_string = {
    { HttpMethod::HEAD,    "HEAD"},
    { HttpMethod::GET,     "GET"},
    { HttpMethod::POST,    "POST"},
    { HttpMethod::PUT,     "PUT"},
    { HttpMethod::DELETE,  "DELETE"},
    { HttpMethod::CONNECT, "CONNECT"},
    { HttpMethod::OPTIONS, "OPTIONS"},
    { HttpMethod::TRACE,   "TRACE"},
    { HttpMethod::PATCH,   "PATCH"},
    { HttpMethod::ANY,     "ANY"}
};

const std::map<std::string, HttpMethod> method_map_from_string = {
    {"HEAD",    HttpMethod::HEAD},
    {"GET",     HttpMethod::GET},
    {"POST",    HttpMethod::POST},
    {"PUT",     HttpMethod::PUT},
    {"DELETE",  HttpMethod::DELETE},
    {"CONNECT", HttpMethod::CONNECT},
    {"OPTIONS", HttpMethod::OPTIONS},
    {"TRACE",   HttpMethod::TRACE},
    {"PATCH",   HttpMethod::PATCH},
    {"ANY",     HttpMethod::ANY}
};

typedef EnumStringifier<HttpVersion,    version_map_to_string, version_map_from_string> Version;
typedef EnumStringifier<HttpStatusCode, status_map_to_string,  status_map_from_string>  StatusCode;
typedef EnumStringifier<HttpMethod,     method_map_to_string,  method_map_from_string>  Method;

} // endof namespace Http

inline std::ostream& operator<<(std::ostream& os, const HttpVersion& value)
{
    return os << Http::version_map_to_string.at(value); // switch-cases omitted
}

inline std::ostream& operator<<(std::ostream& os, const HttpStatusCode& value)
{
    return os << Http::status_map_to_string.at(value); // switch-cases omitted
}

inline std::ostream& operator<<(std::ostream& os, const HttpMethod& value)
{
    return os << Http::method_map_to_string.at(value); // switch-cases omitted
}

#endif
