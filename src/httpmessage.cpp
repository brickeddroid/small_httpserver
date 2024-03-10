#include <httpmessage.hpp>
#include <iostream>
#include <regex>

bool HttpMessage::include_head() { return !m_body_only || m_header_only; }
bool HttpMessage::include_body() { return m_body_only || !m_header_only; }

void HttpMessage::set_head_only(bool h_only) { m_header_only = h_only; }
void HttpMessage::set_body_only(bool b_only) { m_body_only = b_only; }

HttpMessage::HttpMessage()
    : m_version(HttpVersion::Http_11),
      m_status(HttpStatusCode::NotImplemented),
      m_header_only(true),
      m_body_only(true)
{}

void HttpMessage::set_version(HttpVersion version){
    m_version = version;
}
HttpVersion HttpMessage::version(){
    return m_version;
}
void HttpMessage::add_header(const std::string& key, const std::string& value){
    m_headers[key] = value;
}
std::string HttpMessage::header(const std::string& key) const {
    if(m_headers.count(key) > 0) return m_headers.at(key);
    return std::string();
}

void HttpMessage::parse_header(const std::string& headers){
    std::istringstream iss(headers);
    std::string head_line;
    while(std::getline(iss, head_line)){

        head_line.erase(std::remove(head_line.begin(), head_line.end(), '\r' ), head_line.end());
        head_line.erase(std::remove(head_line.begin(), head_line.end(), '\n' ), head_line.end());
        int delimiter = head_line.find(':');
        if(delimiter == -1) break;
        std::string key = head_line.substr(0, delimiter);
        std::string value = head_line.substr(delimiter + 2);
        add_header(key, value);
    }
}

void HttpMessage::set_content(const std::string& content){
    m_content = std::move(content);
}

const std::string& HttpMessage::content(){
    return m_content;
}

std::string HttpMessage::to_string(bool includ_content){
    std::ostringstream oss;
    if(include_head()){
        oss << headline() << "\r\n";
        for(const auto& it : m_headers){
            oss << it.first << ": " << it.second << "\r\n";
        }
        oss << "\r\n";
    }
    if(include_body() || includ_content){
        oss << m_content;
    }
    return oss.str();
}

void HttpMessage::from_string(const std::string& http_message){
    std::istringstream iss(http_message);
    std::string head_line;
    std::getline(iss, head_line);
    int lpos = 0,
        rpos = http_message.find("\r\n");

    parse_headline(http_message.substr(lpos, rpos));
    lpos = rpos + 2;
    rpos = http_message.find("\r\n\r\n");
    parse_header(http_message.substr(lpos, rpos));
    lpos = rpos + 4;
    set_content(http_message.substr(lpos));
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
    m_method = Http::Method::from_string(method);
    std::istringstream p_iss(path);
    std::getline(p_iss, m_path, '?');
    while(std::getline(p_iss, query_string, '&')){
        int pos = query_string.find('=');
        std::string pkey = query_string.substr(0, pos);
        std::string pvalue = query_string.substr(pos+1);
        add_query_parameter(pkey, pvalue);
    }
    std::cout << "Request Method: " << method << "|Path: " << m_path << "|Version: " << version << std::endl;
}

std::string HttpRequest::headline(){
    std::ostringstream oss;
    oss << Http::Method::to_string(m_method) << " "
        << m_path  << " "
        << Http::Version::to_string(m_version);
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
{
}

void HttpResponse::parse_headline(const std::string& head_line){
    std::string version, status, status_string;
    std::istringstream iss(head_line);
    iss >> version >> status >> status_string;
    m_version = Http::Version::from_string(version);
    m_status  = (HttpStatusCode)std::stoi(status);
    std::cout << "Response version: " << version << "|status: " << status << "|status_s: " << status_string << std::endl;
}

std::string HttpResponse::headline(){
    std::ostringstream oss;
    oss << Http::Version::to_string(m_version) << " "
        << std::to_string((int)m_status)  << " "
        << Http::StatusCode::to_string(m_status);
    return oss.str();
}

HttpStatusCode HttpResponse::status_code(){
    return m_status;
}

void HttpResponse::set_status_code(HttpStatusCode status){
    m_status = status;
}
