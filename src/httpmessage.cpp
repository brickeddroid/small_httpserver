#include "../include/httpmessage.hpp"
#include <iostream>

void exitWithError(const char* msg){
    std::cout << "ERROR: " << msg << std::endl;
    exit(1);
}

HttpMessage::HttpMessage()
    : m_version(HttpVersion::Http_10),
      m_status(HttpStatusCode::NotImplemented)
{}

void HttpMessage::set_version(HttpVersion version){
    m_version = version;
}
HttpVersion HttpMessage::version(){
    return m_version;
}
void HttpMessage::set_status_code(HttpStatusCode status){
    m_status = status;
}
HttpStatusCode HttpMessage::status_code(){
    return m_status;
}
void HttpMessage::add_header(const std::string& key, const std::string& value){
    m_headers[key] = value;
}
std::string HttpMessage::header(const std::string& key) const {
    if(m_headers.count(key) > 0) return m_headers.at(key);
    return std::string();
}
void HttpMessage::set_content(const std::string& content){
    m_content = std::move(content);
}
const std::string& HttpMessage::content(){
    return m_content;
}

std::string HttpMessage::to_string(bool include_content){
    std::ostringstream oss;
    oss << headline() << "\r\n";
    for(const auto& it : m_headers){
        oss << it.first << ": " << it.second << "\r\n";
    }
    oss << "\r\n";
    if(include_content){
        oss << m_content;
    }
    return oss.str();
}

void HttpMessage::from_string(const std::string& http_message){
    std::istringstream iss(http_message);
    std::string head_line;
    std::getline(iss, head_line);
    parse_headline(head_line);
    for(std::string line; std::getline(iss, line);){
        std::cout << "LINE " << line << std::endl;
        if(line[0] == 13){
            continue;
        }
        int pos = line.find(':');
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos+2);
        add_header(key, value);
    }
    int lpos = http_message.find("\r\n\r\n", 0) + 4;
    std::string body = http_message.substr(lpos);
    std::cout << "BODY: " << body << std::endl;
}


HttpRequest::HttpRequest()
    : HttpMessage()
{}

void HttpRequest::add_query_parameter(const std::string& key, const std::string& value){
    m_query_parameters[key] = value;
}

std::string HttpRequest::query_parameter(const std::string& key) const {
    if(m_query_parameters.count(key) > 0) return m_query_parameters.at(key);
    return std::string();
}

void HttpRequest::parse_headline(const std::string& head_line){
    std::string method, path, version, query_string;
    std::istringstream iss(head_line);
    iss >> method >> path >> version;
    m_method = method_from_string(method);
    std::istringstream piss(path);
    std::getline(piss, m_path, '?');
    while(std::getline(piss, query_string, '&')){
        int pos = query_string.find('=');
        std::string pkey = query_string.substr(0, pos);
        std::string pvalue = query_string.substr(pos+1);
        add_query_parameter(pkey, pvalue);
    }
    std::cout << "Request Method: " << method << "|Path: " << m_path << "|Version: " << version << std::endl;
}

std::string HttpRequest::headline(){
    std::ostringstream oss;
    oss << method_to_string(m_method) << " "
        << m_path  << " "
        << version_to_string(m_version);
    return oss.str();
}

const std::string& HttpRequest::path() const {
    return m_path;
}

const HttpMethod& HttpRequest::method() const {
    return m_method;
}


HttpResponse::HttpResponse()
    : HttpMessage()
{}

void HttpResponse::parse_headline(const std::string& head_line){
    std::string version, status, status_string;
    std::istringstream iss(head_line);
    iss >> version >> status >> status_string;
    std::cout << "Response version: " << version << "|status: " << status << "|status_s: " << status_string << std::endl;
}

std::string HttpResponse::headline(){
    std::ostringstream oss;
    oss << version_to_string(m_version) << " "
        << std::to_string((int)m_status)  << " "
        << status_to_string(m_status);
    return oss.str();
}
