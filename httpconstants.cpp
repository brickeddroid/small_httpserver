#include "httpconstants.hpp"

namespace YatePlugin {

const std::map<HttpVersion, std::string> http_version_map_to_string = {
    { HttpVersion::Http_09, "HTTP/0.9" },
    { HttpVersion::Http_10, "HTTP/1.0" },
    { HttpVersion::Http_11, "HTTP/1.1" },
    { HttpVersion::Http_20, "HTTP/2.0" },
    { HttpVersion::Http_30, "HTTP/3.0" }
};

const std::map<HttpStatusCode, std::string> http_status_map_to_string = {
    {HttpStatusCode::Continue, "Continue"},
    {HttpStatusCode::SwitchingProtocols, "SwitchingProtocols"},
    {HttpStatusCode::EarlyHints, "EarlyHints"},
    {HttpStatusCode::Ok, "Ok"},
    {HttpStatusCode::Created, "Created"},
    {HttpStatusCode::Accepted, "Accepted"},
    {HttpStatusCode::NonAuthoritativeInformation, "NonAuthoritativeInformation"},
    {HttpStatusCode::NoContent, "NoContent"},
    {HttpStatusCode::ResetContent, "ResetContent"},
    {HttpStatusCode::PartialContent, "PartialContent"},
    {HttpStatusCode::MultipleChoices, "MultipleChoices"},
    {HttpStatusCode::MovedPermanently, "MovedPermanently"},
    {HttpStatusCode::Found, "Found"},
    {HttpStatusCode::NotModified, "NotModified"},
    {HttpStatusCode::BadRequest, "BadRequest"},
    {HttpStatusCode::Unauthorized, "Unauthorized"},
    {HttpStatusCode::Forbidden, "Forbidden"},
    {HttpStatusCode::NotFound, "NotFound"},
    {HttpStatusCode::MethodNotAllowed, "MethodNotAllowed"},
    {HttpStatusCode::RequestTimeout, "RequestTimeout"},
    {HttpStatusCode::ImATeapot, "ImATeapot"},
    {HttpStatusCode::InternalServerError, "InternalServerError"},
    {HttpStatusCode::NotImplemented, "NotImplemented"},
    {HttpStatusCode::BadGateway, "BadGateway"},
    {HttpStatusCode::ServiceUnvailable, "ServiceUnvailable"},
    {HttpStatusCode::GatewayTimeout, "GatewayTimeout"},
    {HttpStatusCode::HttpVersionNotSupported, "HttpVersionNotSupported"}
};

const std::map<HttpMethod, std::string> http_method_map_to_string = {
    {HttpMethod::HEAD,    "HEAD"},
    {HttpMethod::GET,     "GET"},
    {HttpMethod::POST,    "POST"},
    {HttpMethod::PUT,     "PUT"},
    {HttpMethod::DELETE,  "DELETE"},
    {HttpMethod::CONNECT, "CONNECT"},
    {HttpMethod::OPTIONS, "OPTIONS"},
    {HttpMethod::TRACE,   "TRACE"},
    {HttpMethod::PATCH,   "PATCH"},
    {HttpMethod::UNKNOWN, "UNKNOWN"}
};

const std::map<std::string, HttpVersion> http_version_map_from_string = {
    { "HTTP/0.9", HttpVersion::Http_09 },
    { "HTTP/1.0", HttpVersion::Http_10 },
    { "HTTP/1",   HttpVersion::Http_20 },
    { "HTTP/1.1", HttpVersion::Http_11 },
    { "HTTP/2.0", HttpVersion::Http_20 },
    { "HTTP/2",   HttpVersion::Http_20 },
    { "HTTP/3.0", HttpVersion::Http_30 }
};

const std::map<std::string, HttpStatusCode> http_status_map_from_string = {
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

const std::map<std::string, HttpMethod> http_method_map_from_string = {
    {"HEAD",    HttpMethod::HEAD},
    {"GET",     HttpMethod::GET},
    {"POST",    HttpMethod::POST},
    {"PUT",     HttpMethod::PUT},
    {"DELETE",  HttpMethod::DELETE},
    {"CONNECT", HttpMethod::CONNECT},
    {"OPTIONS", HttpMethod::OPTIONS},
    {"TRACE",   HttpMethod::TRACE},
    {"PATCH",   HttpMethod::PATCH},
    {"UNKNOWN", HttpMethod::UNKNOWN}
};


std::string version_to_string(HttpVersion version) {
    if(http_version_map_to_string.count(version) == 0) {
        return "VERSION UNKNOWN";
    }
    return http_version_map_to_string.at(version);
}

std::string status_to_string(HttpStatusCode status) {
    if(http_status_map_to_string.count(status) == 0) {
        return http_status_map_to_string.at(HttpStatusCode::Forbidden);
    }
    return http_status_map_to_string.at(status);
}

std::string method_to_string(HttpMethod method){
    if(http_method_map_to_string.count(method) == 0) {
        return http_method_map_to_string.at(HttpMethod::UNKNOWN);
    }
    return http_method_map_to_string.at(method);
}

HttpVersion version_from_string(const std::string& ver_str) {
    if(http_version_map_from_string.count(ver_str) == 0) {
        return HttpVersion::UNKNOWN;
    }
    return http_version_map_from_string.at(ver_str);
}

HttpStatusCode status_from_string(const std::string& sta_str){
    if(http_status_map_from_string.count(sta_str) == 0) {
        return HttpStatusCode::ImATeapot;
    }
    return http_status_map_from_string.at(sta_str);
}

HttpMethod method_from_string(const std::string& met_str){
    if(http_method_map_from_string.count(met_str) == 0) {
        return HttpMethod::UNKNOWN;
    }
    return http_method_map_from_string.at(met_str);
}

} // endof namespace YatePlugin
